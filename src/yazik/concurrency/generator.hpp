#pragma once

#include <type_traits>
#include <utility>
#include <exception>
#include <iterator>
#include <functional>

#include <yazik/utility/detail/promise_base.hpp>
#include <yazik/utility/result.hpp>

// from https://github.com/lewissbaker/cppcoro/blob/master/include/cppcoro/generator.hpp

namespace yazik {

    template<typename T, typename Error = string>
	class Generator;

namespace promises {

    template<typename T, typename Error>
    class GeneratorPromise
    : public ErrorPropagationPromise<Error> {
    public:
        using value_type = std::remove_reference_t<T>;
        using reference_type = std::conditional_t<std::is_reference_v<T>, T, T&>;
        using pointer_type = value_type*;
    private:

        enum class State { Empty, Value, Errorred };
        State _state = State::Empty;

        union {
            pointer_type _value;
            Error _error;
        };

    public:

        GeneratorPromise() noexcept {};
        ~GeneratorPromise() {
            if (_state == State::Value) {
                _error.~Error();
            }
        }

        Generator<T, Error> get_return_object() noexcept;

        constexpr std::experimental::suspend_always initial_suspend() const { return {}; }
        constexpr std::experimental::suspend_always final_suspend() const { return {}; }

        template<
            typename U = T,
            std::enable_if_t<!std::is_rvalue_reference<U>::value, int> = 0
        >
        std::experimental::suspend_always yield_value(std::remove_reference_t<T>& value) noexcept {
            _state = State::Value;
            _value = std::addressof(value);
            return {};
        }

        std::experimental::suspend_always yield_value(std::remove_reference_t<T>&& value) noexcept {
            _state = State::Value;
            _value = std::addressof(value);
            return {};
        }

        bool can_continue() {
            return _state == State::Value;
        }

        bool contain_error() {
            return _state == State::Errorred;
        }

        void set_error(Error&& error) noexcept {
            _state = State::Errorred;
            new (&_error) Error (std::forward<Error>(error));
        }

        void unhandled_exception() {
            set_error(::yazik::detail::current_exception_to_error<Error>());
        }

        void propagate(std::experimental::coroutine_handle<>& self_h) override {
            /* noop */
            $breakpoint_hint
        }

        void propagate_error(Error&& error, std::experimental::coroutine_handle<>& self_h) override {
            set_error(std::forward<Error>(error));
//            self_h.destroy();
        }

        void return_void() {}

        reference_type value() const noexcept {
            return static_cast<reference_type>(*_value);
        }

        template <typename U, typename UError>
        auto await_transform(::folly::Expected<U, UError>&& value) {
            return std::forward<Result<U, UError>>(value);
        }

        template <typename UError>
        auto await_transform(::folly::Unexpected<UError>&& failure) {
            return Result<void, UError>(std::forward<::folly::Unexpected<UError>>(failure));
        }

        Result<void, Error> result() {
            if (_state == State::Errorred)
                return yaz_fail(_error);

            return yaz_ok<Error>();
        }
    };

    struct GeneratorSentinel {};

    template<typename T, typename Error>
    class GeneratorIterator {
        using coroutine_handle = std::experimental::coroutine_handle<GeneratorPromise<T, Error>>;
        coroutine_handle _coroutine;
    public:

        using iterator_category = std::input_iterator_tag;
        // What type should we use for counting elements of a potentially infinite sequence?
        using difference_type = std::ptrdiff_t;
        using value_type = typename GeneratorPromise<T, Error>::value_type;
        using reference = typename GeneratorPromise<T, Error>::reference_type;
        using pointer = typename GeneratorPromise<T, Error>::pointer_type;

        // Iterator needs to be default-constructible to satisfy the Range concept.
        GeneratorIterator() noexcept
        : _coroutine { nullptr }
        {}

        explicit GeneratorIterator(coroutine_handle coroutine) noexcept
        : _coroutine { coroutine }
        {}

        friend bool operator==(const GeneratorIterator& it, GeneratorSentinel) noexcept {
            return !it._coroutine || it._coroutine.done() || !it._coroutine.promise().can_continue();
        }

        friend bool operator!=(const GeneratorIterator& it, GeneratorSentinel s) noexcept {
            return !(it == s);
        }

        friend bool operator==(GeneratorSentinel s, const GeneratorIterator& it) noexcept {
            return (it == s);
        }

        friend bool operator!=(GeneratorSentinel s, const GeneratorIterator& it) noexcept {
            return it != s;
        }

        GeneratorIterator& operator++() {
            _coroutine.resume();
            return *this;
        }

        // Need to provide post-increment operator to implement the 'Range' concept.
        void operator++(int) {
            (void)operator++();
        }

        reference operator*() const noexcept {
            return _coroutine.promise().value();
        }

        pointer operator->() const noexcept {
            return std::addressof(operator*());
        }

    };
} // end of promises namespace

	template<typename T, typename Error>
	class [[nodiscard]] Generator {
	public:
	    using promise_type = promises::GeneratorPromise<T, Error>;
		using iterator = promises::GeneratorIterator<T, Error>;

	private:
        friend class promises::GeneratorPromise<T, Error>;

        std::experimental::coroutine_handle<promise_type> _coroutine;

		explicit Generator(std::experimental::coroutine_handle<promise_type> coroutine) noexcept
        : _coroutine(coroutine)
		{}

	public:

		Generator() noexcept
        : _coroutine(nullptr)
		{}

		Generator(Generator&& other) noexcept
        : _coroutine(other._coroutine) {
			other._coroutine = nullptr;
		}

		Generator(const Generator& other) = delete;

		~Generator() {
			if (_coroutine) {
				_coroutine.destroy();
			}
		}

		Generator& operator=(Generator other) noexcept {
			swap(other);
			return *this;
		}

		iterator begin() {
			if (_coroutine) {
				_coroutine.resume();
			}

			return iterator{ _coroutine };
		}

		promises::GeneratorSentinel end() noexcept {
			return promises::GeneratorSentinel{};
		}

		void swap(Generator& other) noexcept {
			std::swap(_coroutine, other._coroutine);
		}

		Result<void, Error> result() {
            return _coroutine.promise().result();
		}
	};

	template<typename T, typename Error>
	void swap(Generator<T, Error>& a, Generator<T, Error>& b) {
		a.swap(b);
	}

namespace promises {
    template<typename T, typename Error>
    Generator<T, Error> GeneratorPromise<T, Error>::get_return_object() noexcept {
        using coroutine_handle = std::experimental::coroutine_handle<GeneratorPromise<T, Error>>;
        return Generator<T, Error>{ coroutine_handle::from_promise(*this) };
    }
} // end of promises namespace

namespace concurrency {

    using ::yazik::Generator;

} // end of concurrency namespace

} // end of ::yazik namespace