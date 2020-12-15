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


    class BasePromise
    : utility::NonCopyable<BasePromise> {
    public:
        virtual ~BasePromise() {}

        virtual void propagate(
            std::experimental::coroutine_handle<>& self_h
        ) = 0;
    };

    template <typename Error>
    class ErrorPropagationPromise
    : public BasePromise {
    public:
        virtual void propagate_error(
            Error&& error,
            std::experimental::coroutine_handle<>& self_h
        ) = 0;
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

    template <
        template <typename, typename> typename PromiseType,
        typename T,
        typename Error,
        typename UError
    > inline void propagate_error(
        std::experimental::coroutine_handle<PromiseType<T, Error>>& h,
        UError&& error
    ) {
        using Promise = PromiseType<T, Error>;

        static_assert(
            std::is_base_of_v<ErrorPropagationPromise<Error>, Promise>,
            "unknown promise type, kill it with fire"
        );

        h.promise().propagate_error(
            detail::ErrorMapper<
                UError, Error
            >::map(
                std::forward<UError>(error)
            ),
            h
        );
    }

    template<typename Promise, typename Error>
    inline void propagate_error(
        std::experimental::coroutine_handle<Promise>& h,
        Error&& error
    ) {
        throw error;
    }

    template <
        template <typename, typename> typename PromiseType,
        typename T,
        typename Error
    > inline void propagate(
        std::experimental::coroutine_handle<PromiseType<T, Error>>& h
    ) {
        using Promise = PromiseType<T, Error>;

        static_assert(
            std::is_base_of_v<BasePromise, Promise>,
            "unknown promise type, kill it with fire"
        );

        h.promise().propagate(h);
    }

} // end of yazik::promises namespace