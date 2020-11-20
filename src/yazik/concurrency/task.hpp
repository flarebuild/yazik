#pragma once

// from https://github.com/lewissbaker/cppcoro/blob/master/include/cppcoro/config.hpp

#include <folly/Function.h>

#include <yazik/utility/detail/promise_base.hpp>
#include <yazik/concurrency/future_result.hpp>

namespace yazik {

    template<typename T = void, typename Error = string> class Task;

namespace promises {

    enum class TaskPromiseState { Empty, Value, Exception };

    template<typename T, typename Error>
    class TaskPromiseBase
    : public ErrorPropagationPromise<Error> {
        std::experimental::coroutine_handle<> _continuation = nullptr;
        unique_function<void(std::experimental::coroutine_handle<>&, Error&&)> _error_propagation;

        struct FinalAwaitable {
            bool await_ready() const noexcept { return false; }

            template<typename Promise>
            std::experimental::coroutine_handle<> await_suspend(
                std::experimental::coroutine_handle<Promise> coro
            ) noexcept {
                return coro.promise()._continuation;
            }

            void await_resume() noexcept {}
        };

        friend struct FinalAwaitable;

    protected:

        TaskPromiseState _state = TaskPromiseState::Empty;

        union {
            std::conditional_t<
                std::is_same_v<T,void>,
                utility::unit_t,
                T
            >  _value;
            Error _error;
        };

        TaskPromiseBase() noexcept {}
    public:

        ~TaskPromiseBase() {
            switch (_state) {
            case TaskPromiseState::Exception:
                _error.~Error();
                break;
            case TaskPromiseState::Value:
                if constexpr (!std::is_same_v<T, void>) {
                    _value.~T();
                }
                break;
            default:
                break;
            }
        }

        auto initial_suspend() noexcept {
            return std::experimental::suspend_always{};
        }

        auto final_suspend() noexcept {
            return FinalAwaitable{};
        }

        void set_continuation(
            std::experimental::coroutine_handle<>& continuation,
            unique_function<void(std::experimental::coroutine_handle<>&, Error&&)>&& error_propagation
        ) noexcept {
            _continuation = continuation;
            _error_propagation = std::move(error_propagation);
        }

        void unhandled_exception() noexcept {
            set_error(detail::current_exception_to_error<Error>());
        }

        template<
            typename UError,
            typename = std::enable_if_t<std::is_convertible_v<UError&&, Error>>
        >void set_error(UError&& e) noexcept(std::is_nothrow_constructible_v<Error, UError&&>) {
            _state = TaskPromiseState::Exception;
            ::new (static_cast<void*>(std::addressof(_error))) Error (std::forward<UError>(e));
        }

        void propagate_error(Error&& error, std::experimental::coroutine_handle<>& self_h) override {
            if (_error_propagation) {
                _error_propagation(_continuation, std::forward<Error>(error));
            } else {
                set_error(std::forward<Error>(error));
                _continuation.resume();
            }
        }

        void propagate(std::experimental::coroutine_handle<>& self_h) override {
            if (_state != TaskPromiseState::Empty
                && this->_continuation
                && !this->_continuation.done()
            )
                this->_continuation.resume();
            else
                self_h.resume();
        }

        std::optional<Error> error() noexcept {
            if (_state != TaskPromiseState::Exception)
                return {};
            return std::move(_error);
        }

        T result() {
            switch (_state) {
            case TaskPromiseState::Exception:
                throw std::move(_error);
            case TaskPromiseState::Value:
                if constexpr (std::is_same_v<T, void>) {
                    return;
                } else {
                    return std::move(_value);
                }
            default:
                throw detail::ErrorMapper<string, Error>::map("empty");
            }
        }

        TaskPromiseState promise_state() {
            return _state;
        }

    };

    template<typename Error>
    class TaskPromise<void, Error> final
    : public TaskPromiseBase<void, Error> {
        using Base = TaskPromiseBase<void, Error>;
    public:
        void return_void() noexcept {
            Base::_state = TaskPromiseState::Value;
        }

        Task<void, Error> get_return_object() noexcept;
    };

    template<typename T, typename Error>
    class TaskPromise final
    : public TaskPromiseBase<T, Error> {
        using Base = TaskPromiseBase<T, Error>;
    public:
        template<
            typename Value,
            typename = std::enable_if_t<std::is_convertible_v<Value&&, T>>
        > void return_value(Value&& value) noexcept(std::is_nothrow_constructible_v<T, Value&&>) {
            Base::_state = TaskPromiseState::Value;
            ::new (static_cast<void*>(std::addressof(Base::_value))) T ( std::forward<Value>(value) );
        }

        Task<T, Error> get_return_object() noexcept;
    };

} // end of promises namespace



    struct OneWayTask {
        struct promise_type {
            std::experimental::suspend_never initial_suspend() noexcept { return {}; }
            std::experimental::suspend_never final_suspend() noexcept { return {}; }
            void unhandled_exception() noexcept {
                ::yazik::L::warn(
                    "❗❗❗❗❗ unhandled_exception: {} ❗❗❗❗",
                    ::yazik::detail::current_exception_to_error<string>()
                );
            }
            OneWayTask get_return_object() noexcept { return {}; }
            void return_void() noexcept {}
        };
    };

    template<typename T, typename Error>
    class TaskAwaitableBase {
        using coroutine_handle_t = std::experimental::coroutine_handle<
            promises::TaskPromise<T, Error>
        >;
    protected:
        coroutine_handle_t _coroutine;
    public:
        TaskAwaitableBase(coroutine_handle_t coroutine) noexcept
        : _coroutine { coroutine }
        {}

        bool await_ready() const noexcept {
            return !_coroutine || _coroutine.done();
        }

        template <
            template<typename, typename> typename PromiseType,
            typename U,
            typename UError
        > std::experimental::coroutine_handle<> await_suspend(
            std::experimental::coroutine_handle<PromiseType<U, UError>> awaiting_coroutine
        ) noexcept {

            if constexpr (std::is_base_of_v<promises::ErrorPropagationPromise<UError>,PromiseType<U, UError>>)
                _coroutine.promise().set_continuation(
                    awaiting_coroutine,
                    [](std::experimental::coroutine_handle<>& h, Error&& err) {
                        promises::propagate_error(
                            (std::experimental::coroutine_handle<PromiseType<U, UError>>&) h,
                            detail::ErrorMapper<Error, UError>::map(std::forward<Error>(err))
                        );
                    }
                );
            else
                _coroutine.promise().set_continuation(
                    awaiting_coroutine,
                    nullptr
                );
            return _coroutine;
        }

        std::experimental::coroutine_handle<> await_suspend(
            std::experimental::coroutine_handle<> awaiting_coroutine
        ) noexcept {
            _coroutine.promise().set_continuation(awaiting_coroutine, nullptr);
            return _coroutine;
        }
    };

    template<typename T, typename Error>
    class TaskAwaitableWhenReady
    : public TaskAwaitableBase<T, Error> {
    public:
        using TaskAwaitableBase<T, Error>::TaskAwaitableBase;
        void await_resume() const noexcept {}
    };

	/// \brief
	/// A task represents an operation that produces a result both lazily
	/// and asynchronously.
	///
	/// When you call a coroutine that returns a task, the coroutine
	/// simply captures any passed parameters and returns exeuction to the
	/// caller. Execution of the coroutine body does not start until the
	/// coroutine is first co_await'ed.
	template<typename T, typename Error>
	class [[nodiscard]] Task {
	public:

		using promise_type = promises::TaskPromise<T, Error>;
		using value_type = T;

	private:
        std::experimental::coroutine_handle<promise_type> _coroutine;

	public:

		Task() noexcept
        : _coroutine { nullptr }
		{}

		explicit Task(std::experimental::coroutine_handle<promise_type> coroutine)
        : _coroutine { coroutine }
		{}

		Task(Task&& t) noexcept
        : _coroutine(t._coroutine) {
			t._coroutine = nullptr;
		}

		/// Disable copy construction/assignment.
		Task(const Task&) = delete;
		Task& operator=(const Task&) = delete;

		/// Frees resources used by this task.
		~Task() {
			if (_coroutine){
				_coroutine.destroy();
			}
		}

		Task& operator=(Task&& other) noexcept {
			if (std::addressof(other) != this) {
				if (_coroutine) {
					_coroutine.destroy();
				}

				_coroutine = other._coroutine;
				other._coroutine = nullptr;
			}

			return *this;
		}

		/// \brief
		/// Query if the task result is complete.
		///
		/// Awaiting a task that is ready is guaranteed not to block/suspend.
		bool is_ready() const noexcept {
			return !_coroutine || _coroutine.done()
			    || _coroutine.promise().promise_state() != promises::TaskPromiseState::Empty;
		}

		auto operator co_await() const & noexcept {
			struct Awaitable
            : TaskAwaitableBase<T, Error> {
				using TaskAwaitableBase<T, Error>::TaskAwaitableBase;

				T await_resume() {
					if (!this->_coroutine) {
						throw promises::broken_promise{};
					}
                    if (auto error = this->_coroutine.promise().error())
                        throw std::move(*error);

                    if constexpr (std::is_same_v<void, T>)
                        return;
                    else
                        return this->_coroutine.promise().result();
				}
			};

			return Awaitable { _coroutine };
		}

		auto operator co_await() const && noexcept {
			struct Awaitable
            : TaskAwaitableBase<T, Error> {
				using TaskAwaitableBase<T, Error>::TaskAwaitableBase;

				T await_resume() {
					if (!this->_coroutine) {
						throw promises::broken_promise{};
					}
                    if (auto error = this->_coroutine.promise().error())
                        throw std::move(*error);

                    if constexpr (std::is_same_v<void, T>)
                        return;
                    else
                        return this->_coroutine.promise().result();
				}
			};

			return Awaitable { _coroutine };
		}

		/// \brief
		/// Returns an awaitable that will await completion of the task without
		/// attempting to retrieve the result.
		TaskAwaitableWhenReady<T, Error> when_ready() const noexcept {
			return { _coroutine };
		}

		auto wrapped() const noexcept {
		    struct Awaitable : TaskAwaitableBase<T, Error> {
				using TaskAwaitableBase<T, Error>::TaskAwaitableBase;

                Result<T, Error> await_resume() {
                    if (!this->_coroutine) {
                        throw promises::broken_promise{};
                    }
                    if (auto error = this->_coroutine.promise().error())
                        return yaz_fail<Error>(std::move(*error));

                    if constexpr (std::is_same_v<void, T>)
                        return yaz_ok<Error>();
                    else
                        return this->_coroutine.promise().result();
                }
		    };
		    return Awaitable { _coroutine };
		}

		Future<T, Error> fut() {
		    Task self_move = std::move(*this);
		    if constexpr (std::is_same_v<T, void>) {
		        co_await std::move(self_move);
		        co_return;
		    } else {
		        co_return co_await std::move(self_move);
		    }
		}

		OneWayTask one_way() {
		    Task self_move = std::move(*this);
		    co_await self_move.when_ready();
            co_return;
		}
	};

    template <typename Error = string>
    Task<void, Error> yaz_ok_t() noexcept {
        co_return;
    }

    template <typename T, typename Error = string>
    Task<T, Error> yaz_ok_t(T&& value) noexcept {
        co_return std::forward<T>(value);
    }

    template <typename T = void, typename Error = string>
    Task<T, Error> yaz_fail_t(Error e) noexcept {
        co_await  yaz_fail(std::move(e));
        __builtin_unreachable();
    }


namespace promises {
    template<typename T, typename Error>
    Task<T, Error> TaskPromise<T, Error>::get_return_object() noexcept {
        return Task<T, Error>{ std::experimental::coroutine_handle<TaskPromise>::from_promise(*this) };
    }

    template<typename Error>
    Task<void, Error> TaskPromise<void, Error>::get_return_object() noexcept {
        return Task<void, Error>{ std::experimental::coroutine_handle<TaskPromise>::from_promise(*this) };
    }
} // end of promises namespace

    template <typename T, typename Error = string>
    using TaskIPtr = Task<intrusive_ptr<T>, Error>;

} // end of ::yazik namespace