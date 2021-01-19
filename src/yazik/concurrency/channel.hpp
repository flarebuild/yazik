#pragma once

#include <yazik/utility/detail/promise_base.hpp>
#include <yazik/utility/lambda_continuation.hpp>
#include <yazik/utility/logging.hpp>
#include <yazik/concurrency/coro_scope.hpp>
#include <yazik/concurrency/future_result.hpp>

// from https://github.com/lewissbaker/cppcoro/blob/master/include/cppcoro/async_generator.hpp

namespace yazik {

    template<typename Payload, typename Error = string>
    class Channel;

namespace promises {

    template <typename Payload, typename Error>
    class ChannelIterator;
    template <typename Payload, typename Error>
    class ChannelAdvanceOp;
    template <typename Error>
    class ChannelYieldOp;

    template <typename Payload, typename Error>
    class ChannelPromise final
    : public ErrorPropagationPromise<Error> {
        friend class ChannelYieldOp<Error>;
        friend class ChannelAdvanceOp<Payload, Error>;

        Payload* _value_ptr = nullptr;
        std::optional<Result<Payload, Error>> _result;

        PromiseContinuationHolder<Error> _continuation;
        ChannelYieldOp<Error> internal_yield_value() noexcept;

    public:
        ChannelPromise() = default;
        ~ChannelPromise() = default;

        std::optional<Result<Payload, Error>>& result() {
            return _result;
        }

        auto initial_suspend() const noexcept{
            return std::experimental::suspend_always{};
        }

        ChannelYieldOp<Error> final_suspend() noexcept;

        void unhandled_exception() {
            auto handle = propagate_error_impl(
                detail::current_exception_to_error<Error>(),
                _continuation.self_handle()
            );
            if (handle && _continuation.can_propagate() && !_continuation.need_rethrow())
                _continuation.change_continuation_handle(handle);
        }

        void set_error(Error&& error) {
            _result = yaz_fail(std::forward<Error>(error));
        }

        void return_void() noexcept{}

        ChannelYieldOp<Error> yield_value(Payload&& payload) noexcept;
        ChannelYieldOp<Error> yield_value(const Payload& payload) noexcept;

        bool need_rethrow() override {
            return !_continuation.can_propagate();
        }

        std::experimental::coroutine_handle<>
        propagate_error_impl(Error&& error, std::experimental::coroutine_handle<>& self_h) override {
            std::experimental::coroutine_handle<> handle;
            if (_continuation.can_propagate()) {
                handle =  _continuation.propagate_error(error);
            } else {
                handle = _continuation.continuation_handle();
            }

            if (_continuation.need_rethrow()) {
                set_error(std::forward<Error>(error));
            }

            return handle;
        }

        PromiseContinuationHolder<Error>& continuation() {
            return _continuation;
        }

        Channel<Payload, Error> get_return_object() noexcept;

        void drop_value_ptr() noexcept {
            _value_ptr = nullptr;
        }

        bool finished() {
            return _value_ptr == nullptr;
        }
    };

    template<typename Error>
    class ChannelYieldOp final {
        PromiseContinuationHolder<Error>& _continuation;
    public:

        ChannelYieldOp(PromiseContinuationHolder<Error>& continuation) noexcept
        : _continuation { continuation }
        {}

        bool await_ready() const noexcept {
            return false;
        }

        std::experimental::coroutine_handle<>
        await_suspend([[maybe_unused]] std::experimental::coroutine_handle<> producer) noexcept {
            return _continuation.continuation_handle();
        }

        void await_resume() noexcept {
            $breakpoint_hint
        }
    };

    template <typename Payload, typename Error>
    inline ChannelYieldOp<Error> ChannelPromise<Payload, Error>::final_suspend() noexcept {
        return internal_yield_value();
    }

    template <typename Payload, typename Error>
    inline ChannelYieldOp<Error> ChannelPromise<Payload, Error>::internal_yield_value() noexcept {
        return ChannelYieldOp{ _continuation };
    }

    template <typename Payload, typename Error>
    inline ChannelYieldOp<Error> ChannelPromise<Payload, Error>::yield_value(
        Payload&& payload
    ) noexcept {
        _result = std::forward<Payload>(payload);
        _value_ptr = &_result->value();
        return internal_yield_value();
    }

    template <typename Payload, typename Error>
    inline ChannelYieldOp<Error> ChannelPromise<Payload, Error>::yield_value(
        const Payload& payload
    ) noexcept {
        _result = payload;
        _value_ptr = &_result->value();
        return internal_yield_value();
    }

    template <typename Payload, typename Error>
    class ChannelAdvanceOp {
    protected:

        std::experimental::coroutine_handle<ChannelPromise<Payload, Error>> _producer_handle;

        ChannelAdvanceOp(
            std::experimental::coroutine_handle<ChannelPromise<Payload, Error>> producer_handle
        ) noexcept
        :  _producer_handle(producer_handle)
        {}

    public:

        bool await_ready() const noexcept { return false; }

        template<typename ConsumerPromise>
        std::experimental::coroutine_handle<>
        await_suspend(std::experimental::coroutine_handle<ConsumerPromise> consumer_handle) {
            auto& promise = _producer_handle.promise();
            if (auto& result = promise.result()) {
                if (result->hasError()) {
                    throw promise.result()->error();
                }
            }

            promise.drop_value_ptr();

            promise.continuation().template set_continuation<ConsumerPromise>(
                (std::experimental::coroutine_handle<>)_producer_handle,
                consumer_handle
            );
            return _producer_handle;
        }
    };

    template<typename Payload, typename Error>
    class ChannelIncrementOp final
    : public ChannelAdvanceOp<Payload, Error> {
        using super = ChannelAdvanceOp<Payload, Error>;

        ChannelIterator<Payload, Error>& _iterator;

    public:

        ChannelIncrementOp(ChannelIterator<Payload, Error>& iterator) noexcept
        : super { iterator._handle }
        , _iterator(iterator)
        {}

        ChannelIterator<Payload, Error>& await_resume();
    };

    template<typename Payload, typename Error>
    class ChannelIterator final {
        using promise_type = ChannelPromise<Payload, Error>;
        using handle_type = std::experimental::coroutine_handle<promise_type>;

        friend class ChannelIncrementOp<Payload, Error>;
        friend class Channel<Payload, Error>;

        handle_type _handle;

    public:

        using iterator_category = std::input_iterator_tag;
        // Not sure what type should be used for difference_type as we don't
        // allow calculating difference between two iterators.
        using difference_type = std::ptrdiff_t;

        using value_type = std::remove_reference_t<Payload>;
        using reference = std::add_lvalue_reference_t<Payload>;
        using pointer = std::add_pointer_t<value_type>;

        ChannelIterator(handle_type handle) noexcept
        : _handle(handle)
        {}

        ChannelIncrementOp<Payload, Error> operator++() noexcept {
            return ChannelIncrementOp<Payload, Error>{ *this };
        }

        reference operator*() const noexcept {
            return _handle.promise().result()->value();
        }

        bool operator==(const ChannelIterator& other) const noexcept {
            if (_handle != other._handle) return false;
            return _handle.promise().finished();
        }

        bool operator!=(const ChannelIterator& other) const noexcept {
            return !(*this == other);
        }

    };

    template<typename Payload, typename Error>
    ChannelIterator<Payload, Error>& ChannelIncrementOp<Payload, Error>::await_resume() {
        return _iterator;
    }

    template<typename Payload, typename Error>
    class ChannelBeginOp final
    : public ChannelAdvanceOp<Payload, Error> {
        using promise_type = ChannelPromise<Payload, Error>;
        using handle_type = std::experimental::coroutine_handle<promise_type>;
        using super = ChannelAdvanceOp<Payload, Error>;
    public:

        ChannelBeginOp(handle_type producer_handle) noexcept
        : super{ producer_handle }
        {}

        using super::await_ready;

        ChannelIterator<Payload, Error> await_resume() {
            return { this->_producer_handle };
        }
    };

} // end of ::yazik::promises namespace

    template<typename Payload, typename Error>
    class [[nodiscard]] ChannelGroup;

    template<typename Payload, typename Error>
    class [[nodiscard]] Channel final {
    public:
        using promise_type = promises::ChannelPromise<Payload, Error>;
        using handle_type = std::experimental::coroutine_handle<promise_type>;
        using result_type = Result<Payload, Error>;
        using iterator = promises::ChannelIterator<Payload, Error>;

    private:

        handle_type _handle;

        Channel() = default;

        friend class ChannelGroup<Payload, Error>;

        inline void destroy() noexcept {
            if (_handle)
                _handle.destroy();
        }

    public:

        explicit Channel(promise_type& promise)
        : _handle { handle_type{ handle_type::from_promise(promise) } }
        {}

        static Channel single(Result<Payload, Error>&& value) {
            return [] (Result<Payload, Error> value) -> Channel {
                if (value) {
                    co_yield std::move(value.value());
                } else {
                    co_await std::move(value);
                }
                co_return;
            } (std::move(value));
        }

        Channel(const Channel &) = delete;

        Channel(Channel &&other) noexcept {
            destroy();
            _handle = other._handle;
            other._handle = nullptr;
        }

        Channel& operator=(const Channel&) = delete;

        Channel& operator=(Channel &&other) noexcept {
            if (std::addressof(other) == this)
                return *this;
            destroy();
            _handle = other._handle;
            other._handle = nullptr;
            return *this;
        }
        ~Channel() {
            destroy();
        }

        auto begin() const noexcept {
            return promises::ChannelBeginOp<Payload, Error>{ _handle };
        }

        auto end() const noexcept {
            return iterator{ _handle };
        }

        void swap(Channel& other) noexcept {
            std::swap(_handle, other._handle);
        }

        Result<void, Error> result() {
            auto& result = _handle.promise().result();
            if (!result) {
                return {};
            }
            if (result->hasError()) {
                return yaz_fail<Error>(std::move(result->error()));
            }
            return yaz_ok<Error>();
        }
    };

    template<typename Payload, typename Error>
    void swap(Channel<Payload, Error>& a, Channel<Payload, Error>& b) noexcept {
        a.swap(b);
    }

namespace promises {
    template<typename Payload, typename Error>
    Channel<Payload, Error> ChannelPromise<Payload, Error>::get_return_object() noexcept {
        return Channel<Payload, Error>{ *this };
    }
} // end of promises namespace

} // end of ::yazik namespace