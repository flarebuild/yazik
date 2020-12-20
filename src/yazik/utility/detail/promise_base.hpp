#pragma once

#include <stdexcept>
#include <experimental/coroutine>

#include <yazik/utility/detail/result_detail.hpp>

namespace yazik::promises {

    /// \brief
	/// Exception thrown when you attempt to retrieve the result of
	/// a task that has been detached from its promise/coroutine.
	class broken_promise
    : public std::logic_error {
	public:
		broken_promise()
        : std::logic_error { "broken promise" }
		{}
	};

	struct ErrorPropagationPromiseMarker{};

    template <typename Error>
    class ErrorPropagationPromise: public ErrorPropagationPromiseMarker {
    public:
        virtual ~ErrorPropagationPromise() = default;
        virtual std::experimental::coroutine_handle<> propagate_error_impl(
            Error&& error,
            std::experimental::coroutine_handle<>& self_h
        ) = 0;

        virtual bool need_rethrow() {
            return false;
        }

        template<typename UError>
        std::experimental::coroutine_handle<> propagate_error(
            UError&& error,
            std::experimental::coroutine_handle<>& self_h
        ) {
            return propagate_error_impl(
                detail::ErrorMapper<UError, Error>::map(std::forward<UError>(error)),
                self_h
            );
        }
    };

    template <
        template<typename, typename> typename PromiseType,
        typename U,
        typename UError
    > concept c_is_error_propagation_promise = std::is_base_of_v<
        promises::ErrorPropagationPromise<UError>,
        PromiseType<U, UError>
    >;

    template <typename T, typename Error>
    struct ResultPromise;
    template <typename T, typename Error>
    class TaskPromise;
    template <typename T, typename Error>
    class FuturePromise;
    template <typename T, typename Error>
    struct GeneratorPromise;
    template <typename T, typename Error>
    struct ChannelPromise;

    template<typename Promise, typename Error>
    std::experimental::coroutine_handle<> propagate_error_templ(
        std::experimental::coroutine_handle<>& continuation,
        Error&& error
    ) {
        if constexpr (!std::is_base_of_v<ErrorPropagationPromiseMarker, Promise>) {
            throw error;
        } else {
            return ((std::experimental::coroutine_handle<Promise>&)continuation).promise().propagate_error(
                std::forward<Error>(error),
                continuation
            );
        }
    }
    template<typename Promise, typename Error>
    std::experimental::coroutine_handle<> propagate_error(
        std::experimental::coroutine_handle<Promise> continuation,
        Error&& error
    ) {
        return propagate_error_templ<Promise, Error>(
            continuation,
            std::forward<Error>(error)
        );
    }


    template <typename Error>
    class PromiseContinuationHolder {
        std::experimental::coroutine_handle<> _self_h;
        std::experimental::coroutine_handle<> _continuation_h;
        using propagate_error_fn = std::experimental::coroutine_handle<> (*)(
            std::experimental::coroutine_handle<>&,
            Error&&
        );
        propagate_error_fn _propagate_error_fn;
        bool _can_propagete = false;
        bool _need_rethrow = false;

    public:

        template <typename Promise>
        void set_continuation(
            std::experimental::coroutine_handle<> self_h,
            std::experimental::coroutine_handle<Promise> continuation_h
        ) {
            _self_h = self_h;
            _continuation_h = continuation_h;
            _propagate_error_fn = propagate_error_templ<Promise, Error>;
            _can_propagete = std::is_base_of_v<ErrorPropagationPromiseMarker, Promise>;
            if constexpr(std::is_base_of_v<ErrorPropagationPromiseMarker, Promise>) {
                _need_rethrow = continuation_h.promise().need_rethrow();
            } else {
                _need_rethrow = true;
            }
        }

        std::experimental::coroutine_handle<>& self_handle() {
            return _self_h;
        }

        std::experimental::coroutine_handle<>& continuation_handle() {
            return _continuation_h;
        }

        void change_continuation_handle(std::experimental::coroutine_handle<> handle) {
            _continuation_h = handle;
        }

        std::experimental::coroutine_handle<> propagate_error(Error error) {
            if (!_continuation_h) {
                throw std::move(error);
            }

            return _propagate_error_fn(
                _continuation_h,
                std::move(error)
            );
        }

        bool can_propagate() {
            return _can_propagete;
        }
        bool need_rethrow() {
            return _need_rethrow;
        }

    };

} // end of yazik::promises namespace