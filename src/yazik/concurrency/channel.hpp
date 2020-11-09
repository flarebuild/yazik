#pragma once

#include <yazik/utility/detail/promise_base.hpp>
#include <yazik/utility/lambda_continuation.hpp>
#include <yazik/utility/logging.hpp>
#include <yazik/concurrency/coro_scope.hpp>
#include <yazik/concurrency/future_result.hpp>

// from https://github.com/lewissbaker/cppcoro/blob/master/include/cppcoro/async_generator.hpp

namespace yazik {

    template<typename Payload, typename Status>
    class Channel;

namespace promises {

    template <typename Payload, typename Status>
    class ChannelIterator;
    template <typename Payload, typename Status>
    class ChannelAdvanceOp;
    class ChannelYieldOp;

    template <typename Payload, typename Status>
    class ChannelPromise final
    : ErrorPropagationPromise<Status> {
        friend class ChannelYieldOp;
        friend class ChannelAdvanceOp<Payload, Status>;

        std::optional<Payload> _payload;
        std::optional<Status> _status;
        std::experimental::coroutine_handle<> _consumer_handle;

        ChannelYieldOp internal_yield_value() noexcept;

    public:
        ChannelPromise() = default;
        ~ChannelPromise() {
//            if (_consumer_handle && !_consumer_handle.done())
//                _consumer_handle.destroy();
        }

        std::optional<Payload>& payload() {
            return _payload;
        }

        std::optional<Status> status() {
            return _status;
        }

        auto initial_suspend() const noexcept{
            return std::experimental::suspend_always{};
        }

        ChannelYieldOp final_suspend() noexcept;

        void unhandled_exception() {
            _status = ::yazik::detail::current_exception_to_error<Status>();
        }

        void set_error(Status&& error) {
            _status = std::forward<Status>(error);
        }

        void return_void() noexcept{}

        ChannelYieldOp yield_value(Payload&& payload) noexcept;
        ChannelYieldOp yield_value(const Payload& payload) noexcept;

        void propagate(std::experimental::coroutine_handle<>& self_h) override {
            self_h.resume();
        }

        void propagate_error(Status&& error, std::experimental::coroutine_handle<>& self_h) override {
            set_error(std::forward<Status>(error));
            if (_consumer_handle && !_consumer_handle.done())
                _consumer_handle.resume();
//            self_h.destroy();
        }

        Channel<Payload, Status> get_return_object() noexcept;
    };

    class ChannelYieldOp final {
        std::experimental::coroutine_handle<> _consumer_handle;
    public:

        ChannelYieldOp(std::experimental::coroutine_handle<> consumer_handle) noexcept
        : _consumer_handle(consumer_handle)
        {}

        bool await_ready() const noexcept {
            return false;
        }

        std::experimental::coroutine_handle<>
        await_suspend([[maybe_unused]] std::experimental::coroutine_handle<> producer) noexcept {
            return _consumer_handle;
        }

        void await_resume() noexcept {}
    };

    template <typename Payload, typename Status>
    inline ChannelYieldOp ChannelPromise<Payload, Status>::final_suspend() noexcept {
        if (!_payload || !_status)
            _status = Status::ok();
        return internal_yield_value();
    }

    template <typename Payload, typename Status>
    inline ChannelYieldOp ChannelPromise<Payload, Status>::internal_yield_value() noexcept {
        return ChannelYieldOp{ _consumer_handle };
    }

    template <typename Payload, typename Status>
    inline ChannelYieldOp ChannelPromise<Payload, Status>::yield_value(Payload&& payload) noexcept {
        _payload.emplace(std::forward<Payload>(payload));
        return internal_yield_value();
    }

    template <typename Payload, typename Status>
    inline ChannelYieldOp ChannelPromise<Payload, Status>::yield_value(const Payload& payload) noexcept {
        _payload = payload;
        return internal_yield_value();
    }

    template <typename Payload, typename Status>
    class ChannelAdvanceOp {
        std::experimental::coroutine_handle<> _producer_handle;

    protected:

        ChannelPromise<Payload, Status>& _promise;

        ChannelAdvanceOp(
            ChannelPromise<Payload, Status>& promise,
            std::experimental::coroutine_handle<> producer_handle
        ) noexcept
        : _promise(promise)
        , _producer_handle(producer_handle)
        {}

    public:

        bool await_ready() const noexcept { return false; }

        std::experimental::coroutine_handle<>
        await_suspend(std::experimental::coroutine_handle<> consumer_handle) noexcept {
            if (_promise._payload)
                _promise._payload.reset();
            _promise._consumer_handle = consumer_handle;
            return _producer_handle;
        }
    };

    template<typename Payload, typename Status>
    class ChannelIncrementOp final
    : public ChannelAdvanceOp<Payload, Status> {
        using super = ChannelAdvanceOp<Payload, Status>;

        ChannelIterator<Payload, Status>& _iterator;

    public:

        ChannelIncrementOp(ChannelIterator<Payload, Status>& iterator) noexcept
        : super { iterator._handle.promise(), iterator._handle }
        , _iterator(iterator)
        {}

        ChannelIterator<Payload, Status>& await_resume();
    };

    template<typename Payload, typename Status>
    class ChannelIterator final {
        using promise_type = ChannelPromise<Payload, Status>;
        using handle_type = std::experimental::coroutine_handle<promise_type>;

        friend class ChannelIncrementOp<Payload, Status>;
        friend class Channel<Payload, Status>;

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

        ChannelIncrementOp<Payload, Status> operator++() noexcept {
            return ChannelIncrementOp<Payload, Status>{ *this };
        }

        reference operator*() const noexcept {
            return *_handle.promise().payload();
        }

        bool operator==(const ChannelIterator& other) const noexcept {
            if (_handle != other._handle) return false;
            return !_handle.promise().payload();
        }

        bool operator!=(const ChannelIterator& other) const noexcept {
            return !(*this == other);
        }

    };

    template<typename Payload, typename Status>
    ChannelIterator<Payload, Status>& ChannelIncrementOp<Payload, Status>::await_resume() {
        return _iterator;
    }

    template<typename Payload, typename Status>
    class ChannelBeginOp final
    : public ChannelAdvanceOp<Payload, Status> {
        using promise_type = ChannelPromise<Payload, Status>;
        using handle_type = std::experimental::coroutine_handle<promise_type>;
        using super = ChannelAdvanceOp<Payload, Status>;
    public:

        ChannelBeginOp(handle_type producer_handle) noexcept
        : super{ producer_handle.promise(), producer_handle }
        {}

        using super::await_ready;

        ChannelIterator<Payload, Status> await_resume() {
            return { handle_type::from_promise(this->_promise) };
        }
    };

} // end of ::yazik::promises namespace

    template<typename Payload, typename Status>
    class [[nodiscard]] ChannelGroup;

    template<typename Payload, typename Status>
    class [[nodiscard]] Channel final {
    public:
        using promise_type = promises::ChannelPromise<Payload, Status>;
        using handle_type = std::experimental::coroutine_handle<promise_type>;
        using result_type = Result<Payload, Status>;
        using iterator = promises::ChannelIterator<Payload, Status>;

    private:
        enum class Type {
            SingleResult,
            Stream
        };
        Type _type;
        union {
            handle_type _handle;
            result_type _result;
        };

        Channel() = default;

        friend class ChannelGroup<Payload, Status>;

        inline void destroy() noexcept {
            switch (_type) {
            case Type::Stream:
                if (_handle)
                    _handle.destroy();
                break;
            case Type::SingleResult:
                _result.~result_type();
                break;
            }
        }

    public:

        explicit Channel(promise_type& promise)
        : _type { Type::Stream }  {
            new (&_handle) handle_type{ handle_type::from_promise(promise) };
        }

        Channel(Result<Payload, Status>&& value)
        : _type { Type::SingleResult } {
            new (&_result) result_type { std::forward<Result<Payload, Status>>(value) };
        }

        Channel(const Channel &) = delete;

        Channel(Channel &&other) noexcept
        : _type { other._type } {
            switch (_type) {
            case Type::Stream:
                _handle = other._handle;
                other._handle = nullptr;
                break;
            case Type::SingleResult:
                new (&_result) result_type { std::move(other._result) };
                break;
            }
        }

        Channel& operator=(const Channel&) = delete;

        Channel& operator=(Channel &&other) noexcept {
            destroy();
            if (std::addressof(other) == this)
                return *this;

            switch (_type) {
            case Type::Stream:
                _handle = other._handle;
                other._handle = nullptr;
                break;
            case Type::SingleResult:
                new (&_result) result_type { std::move(other._result) };
                break;
            }
            return *this;
        }
        ~Channel() {
            destroy();
        }

        result_type* one_result_only() {
            switch (_type) {
            case Type::Stream:
                return nullptr;
                break;
            case Type::SingleResult:
                return &_result;
                break;
            }
        }

        auto begin() const noexcept {
            return promises::ChannelBeginOp<Payload, Status>{ _handle };
        }

        auto end() const noexcept {
            return iterator{ _handle };
        }

        void swap(Channel& other) noexcept {
            std::swap(_result, other._result);
            std::swap(_handle, other._handle);
        }

        Status status() {
            auto sts = _handle.promise().status();
            return *sts;
        }
    };

    template<typename Payload, typename Status>
    class [[nodiscard]] ChannelGroup {
        using channel_t = Channel<Payload, Status>;
        using channels_vec_t = vector<channel_t>;
        channels_vec_t _channels;
        concurrency::executor_ptr_t _executor;

        template <typename Fn>
        static Task<void, Status> payload_stream_accept_impl(Fn clbk, channel_t& in_channel) {
            auto channel = channel_t{};
            channel.swap(in_channel);
            for co_await(auto&& payload: channel)
                payload(clbk).one_way();

            co_return;
        }

        template <typename Fn>
        static Task<void, Status> accept_impl(
            channels_vec_t&& channels,
            concurrency::Executor* executor,
            Fn&& accept_fn
        ) {
            if (channels.empty()) co_await yaz_fail("no_channels");
            auto scope = concurrency::CoroScope {};
            for (auto& channel: channels)
                accept_fn(channel) >> scope;

            co_await scope.join(executor);
            co_return;
        }

    public:

        ChannelGroup(
            channels_vec_t&& channels,
            const concurrency::executor_ptr_t& executor
        )
        : _channels { std::forward<channels_vec_t>(channels) }
        , _executor { executor }
        {}

        template <typename Fn>
        Task<void, Status> accept(Fn&& accept_fn) {
            return accept_impl(
                std::move(_channels),
                _executor.get(),
                std::forward<Fn>(accept_fn)
            );
        }

        template <typename Fn>
        Task<void, Status> operator >> (Fn clbk) {
            return accept(
                [_l_move(clbk), this](channel_t& channel) mutable -> Task<void, Status> {
                    return payload_stream_accept_impl<Fn>(
                        std::move(clbk),
                        channel
                    );
                }
            );
        }
    };

    template<typename Payload, typename Status>
    void swap(Channel<Payload, Status>& a, Channel<Payload, Status>& b) noexcept {
        a.swap(b);
    }

namespace promises {
    template<typename Payload, typename Status>
    Channel<Payload, Status> ChannelPromise<Payload, Status>::get_return_object() noexcept {
        return Channel<Payload, Status>{ *this };
    }
} // end of promises namespace

} // end of ::yazik namespace