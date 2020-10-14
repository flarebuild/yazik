#pragma once

// from https://github.com/lewissbaker/cppcoro/blob/master/include/cppcoro/sequence_range.hpp

#include <type_traits>
#include <memory>
#include <atomic>
#include <cassert>
#include <cstdint>
#include <limits>

#include <yazik/concurrency/executor.hpp>

#if CPPCORO_COMPILER_MSVC
# define CPPCORO_CPU_CACHE_LINE std::hardware_destructive_interference_size
#else
// On most architectures we can assume a 64-byte cache line.
# define CPPCORO_CPU_CACHE_LINE 64
#endif

namespace yazik::concurrency {

namespace detail {

    template<typename T>
	struct manual_lifetime {
        union {
			T _value;
		};
	public:
		manual_lifetime() noexcept {}
		~manual_lifetime() noexcept {}

		manual_lifetime(const manual_lifetime&) = delete;
		manual_lifetime(manual_lifetime&&) = delete;
		manual_lifetime& operator=(const manual_lifetime&) = delete;
		manual_lifetime& operator=(manual_lifetime&&) = delete;

		template<typename... Args>
		std::enable_if_t<std::is_constructible_v<T, Args&&...>> construct(Args&&... args)
        noexcept(std::is_nothrow_constructible_v<T, Args&&...>) {
			::new (static_cast<void*>(std::addressof(_value))) T { static_cast<Args&&>(args)... };
		}

		void destruct() noexcept(std::is_nothrow_destructible_v<T>) {
			_value.~T();
		}

		std::add_pointer_t<T> operator->() noexcept { return std::addressof(**this); }
		std::add_pointer_t<const T> operator->() const noexcept { return std::addressof(**this); }

		T& operator*() & noexcept { return _value; }
		const T& operator*() const & noexcept { return _value; }
		T&& operator*() && noexcept { return static_cast<T&&>(_value); }
		const T&& operator*() const && noexcept { return static_cast<const T&&>(_value); }
	};

	template<typename T>
	struct manual_lifetime<T&> {
        T* _value;
	public:
		manual_lifetime() noexcept {}
		~manual_lifetime() noexcept {}

		manual_lifetime(const manual_lifetime&) = delete;
		manual_lifetime(manual_lifetime&&) = delete;
		manual_lifetime& operator=(const manual_lifetime&) = delete;
		manual_lifetime& operator=(manual_lifetime&&) = delete;

		void construct(T& value) noexcept {
			_value = std::addressof(value);
		}

		void destruct() noexcept {}

		T* operator->() noexcept { return _value; }
		const T* operator->() const noexcept { return _value; }

		T& operator*() noexcept { return *_value; }
		const T& operator*() const noexcept { return *_value; }
	};

	template<typename T>
	struct manual_lifetime<T&&> {
	    T* _value;
	public:
		manual_lifetime() noexcept {}
		~manual_lifetime() noexcept {}

		manual_lifetime(const manual_lifetime&) = delete;
		manual_lifetime(manual_lifetime&&) = delete;
		manual_lifetime& operator=(const manual_lifetime&) = delete;
		manual_lifetime& operator=(manual_lifetime&&) = delete;

		void construct(T&& value) noexcept {
			_value = std::addressof(value);
		}

		void destruct() noexcept {}

		T* operator->() noexcept { return _value; }
		const T* operator->() const noexcept { return _value; }

		T& operator*() & noexcept { return *_value; }
		const T& operator*() const & noexcept { return *_value; }
		T&& operator*() && noexcept { return static_cast<T&&>(*_value); }
		const T&& operator*() const && noexcept { return static_cast<const T&&>(*_value); }

	};

	template<>
	struct manual_lifetime<void> {
		void construct() noexcept {}
		void destruct() noexcept {}
		void operator*() const noexcept {}
	};

	// Helper type that can be cast-to from any type.
    struct any {
        template<typename T>
        any(T&&) noexcept
        {}
    };

    template<typename T>
    struct is_coroutine_handle
        : std::false_type
    {};

    template<typename PROMISE>
    struct is_coroutine_handle<std::experimental::coroutine_handle<PROMISE>>
        : std::true_type
    {};

    // NOTE: We're accepting a return value of coroutine_handle<P> here
    // which is an extension supported by Clang which is not yet part of
    // the C++ coroutines TS.
    template<typename T>
    struct is_valid_await_suspend_return_value : std::disjunction<
        std::is_void<T>,
        std::is_same<T, bool>,
        is_coroutine_handle<T>>
    {};

    template<typename T, typename = std::void_t<>>
    struct is_awaiter : std::false_type {};

    // NOTE: We're testing whether await_suspend() will be callable using an
    // arbitrary coroutine_handle here by checking if it supports being passed
    // a coroutine_handle<void>. This may result in a false-result for some
    // types which are only awaitable within a certain context.
    template<typename T>
    struct is_awaiter<T, std::void_t<
        decltype(std::declval<T>().await_ready()),
        decltype(std::declval<T>().await_suspend(std::declval<std::experimental::coroutine_handle<>>())),
        decltype(std::declval<T>().await_resume())>> :
        std::conjunction<
            std::is_constructible<bool, decltype(std::declval<T>().await_ready())>,
            detail::is_valid_await_suspend_return_value<
                decltype(std::declval<T>().await_suspend(std::declval<std::experimental::coroutine_handle<>>()))>>
    {};

    template<typename T>
    auto get_awaiter_impl(T&& value, int)
        noexcept(noexcept(static_cast<T&&>(value).operator co_await()))
        -> decltype(static_cast<T&&>(value).operator co_await())
    {
        return static_cast<T&&>(value).operator co_await();
    }

    template<typename T>
    auto get_awaiter_impl(T&& value, long)
        noexcept(noexcept(operator co_await(static_cast<T&&>(value))))
        -> decltype(operator co_await(static_cast<T&&>(value)))
    {
        return operator co_await(static_cast<T&&>(value));
    }

    template<
        typename T,
        std::enable_if_t<is_awaiter<T&&>::value, int> = 0
    > T&& get_awaiter_impl(T&& value, any) noexcept {
        return static_cast<T&&>(value);
    }

    template<typename T>
    auto get_awaiter(T&& value)
        noexcept(noexcept(get_awaiter_impl(static_cast<T&&>(value), 123)))
        -> decltype(get_awaiter_impl(static_cast<T&&>(value), 123))
    {
        return detail::get_awaiter_impl(static_cast<T&&>(value), 123);
    }

	template<typename T, typename = void>
	struct awaitable_traits
	{};

	template<typename T>
	struct awaitable_traits<T, std::void_t<decltype(get_awaiter(std::declval<T>()))>> {
		using awaiter_t = decltype(get_awaiter(std::declval<T>()));

		using await_result_t = decltype(std::declval<awaiter_t>().await_resume());
	};

} // end of detail namespace

    template<typename Sequence>
	struct SequenceTraits {
		using value_type = Sequence;
		using difference_type = std::make_signed_t<Sequence>;
		using size_type = std::make_unsigned_t<Sequence>;

		static constexpr value_type initial_sequence = static_cast<value_type>(-1);

		static constexpr difference_type difference(value_type a, value_type b) {
			return static_cast<difference_type>(a - b);
		}

		static constexpr bool precedes(value_type a, value_type b) {
			return difference(a, b) < 0;
		}
	};

    template<typename Sequence, typename Traits>
	class SequenceBarrierWaitOperationBase;

	template<
		typename Sequence = std::size_t,
		typename Traits = SequenceTraits<Sequence>
    > class SequenceBarrierWaitOperation;

	/// A sequence barrier is a synchronisation primitive that allows a single-producer
	/// and multiple-consumers to coordinate with respect to a monotonically increasing
	/// sequence number.
	///
	/// A single producer advances the sequence number by publishing new sequence numbers in a
	/// monotonically increasing order. One or more consumers can query the last-published
	/// sequence number and can wait until a particular sequence number has been published.
	///
	/// A sequence barrier can be used to represent a cursor into a thread-safe producer/consumer
	/// ring-buffer.
	///
	/// See the LMAX Disruptor pattern for more background:
	/// https://lmax-exchange.github.io/disruptor/files/Disruptor-1.0.pdf
	template<
		typename Sequence = std::size_t,
		typename Traits = SequenceTraits<Sequence>
    > class SequenceBarrier {
		static_assert(
			std::is_integral_v<Sequence>,
			"sequence_barrier requires an integral sequence type"
        );

		using awaiter_t = SequenceBarrierWaitOperationBase<Sequence, Traits>;

		friend class SequenceBarrierWaitOperationBase<Sequence, Traits>;

#if CPPCORO_COMPILER_MSVC
# pragma warning(push)
# pragma warning(disable : 4324) // C4324: structure was padded due to alignment specifier
#endif

		// First cache-line is written to by the producer only
		alignas(CPPCORO_CPU_CACHE_LINE)
		std::atomic<Sequence> _last_published;

		// Second cache-line is written to by both the producer and consumers
		alignas(CPPCORO_CPU_CACHE_LINE)
		mutable std::atomic<awaiter_t*> _awaiters;

#if CPPCORO_COMPILER_MSVC
# pragma warning(pop)
#endif

		void add_awaiter(awaiter_t* awaiter) const noexcept;

	public:

		/// Construct a sequence barrier with the specified initial sequence number
		/// as the initial value 'last_published()'.
		SequenceBarrier(Sequence initial_sequence = Traits::initial_sequence) noexcept
        : _last_published { initial_sequence }
        , _awaiters { nullptr }
		{}

		~SequenceBarrier() {
			// Shouldn't be destructing a sequence barrier if there are still waiters.
			assert(_awaiters.load(std::memory_order_relaxed) == nullptr);
		}

		/// Query the sequence number that was most recently published by the producer.
		///
		/// You can assume that all sequence numbers prior to the returned sequence number
		/// have also been published. This means you can safely access all elements with
		/// sequence numbers up to and including the returned sequence number without any
		/// further synchronisation.
		Sequence last_published() const noexcept {
			return _last_published.load(std::memory_order_acquire);
		}

		/// Wait until a particular sequence number has been published.
		///
		/// If the specified sequence number is not yet published then the awaiting coroutine
		/// will be suspended and later resumed inside the call to publish() that publishes
		/// the specified sequence number.
		///
		/// \param targetSequence
		/// The sequence number to wait for.
		///
		/// \return
		/// An awaitable that when co_await'ed will suspend the awaiting coroutine until
		/// the specified target sequence number has been published.
		/// The result of the co_await expression will be the last-known published sequence
		/// number. This is guaranteed not to precede \p targetSequence but may be a sequence
		/// number after \p targetSequence, which indicates that more elements have been
		/// published than you were waiting for.
		[[nodiscard]]
		SequenceBarrierWaitOperation<Sequence, Traits> wait_until_published(
			Sequence target_sequence,
			EnsureOnExecutor* executor
        ) const noexcept {
            return SequenceBarrierWaitOperation<Sequence, Traits> (
                *this,
                target_sequence,
                executor
            );
        }

		/// Publish the specified sequence number to consumers.
		///
		/// This publishes all sequence numbers up to and including the specified sequence
		/// number. This will resume any coroutine that was suspended waiting for a sequence
		/// number that was published by this operation.
		///
		/// \param sequence
		/// The sequence number to publish. This number must not precede the current
		/// last_published() value. ie. the published sequence numbers must be monotonically
		/// increasing.
		void publish(Sequence sequence) noexcept;

	};

	template<typename Sequence, typename Traits>
	class SequenceBarrierWaitOperationBase {
	protected:

		friend class SequenceBarrier<Sequence, Traits>;

		void resume() noexcept {
			// This synchronises with the exchange(true, std::memory_order_acquire) in await_suspend().
			if (_ready_to_resume.exchange(true, std::memory_order_release)) {
				resume_impl();
			}
		}

		virtual void resume_impl() noexcept = 0;

		const SequenceBarrier<Sequence, Traits>& _barrier;
		const Sequence _target_sequence;
		Sequence _last_known_published;
		SequenceBarrierWaitOperationBase* _next;
		std::experimental::coroutine_handle<> _awaiting_coroutine;
		std::atomic<bool> _ready_to_resume;
	public:

		explicit SequenceBarrierWaitOperationBase(
			const SequenceBarrier<Sequence, Traits>& barrier,
			Sequence target_sequence
        ) noexcept
        : _barrier { barrier }
        , _target_sequence { target_sequence }
        , _last_known_published { barrier.last_published() }
        , _ready_to_resume { false }
		{}

		SequenceBarrierWaitOperationBase(
			const SequenceBarrierWaitOperationBase& other
        ) noexcept
        : _barrier { other._barrier }
        , _target_sequence { other._target_sequence }
        , _last_known_published { other._last_known_published }
        , _ready_to_resume { false }
		{}

		bool await_ready() const noexcept {
			return !Traits::precedes( _last_known_published, _target_sequence );
		}

		bool await_suspend(std::experimental::coroutine_handle<> awaiting_coroutine) noexcept {
			_awaiting_coroutine = awaiting_coroutine;
			_barrier.add_awaiter(this);
			return !_ready_to_resume.exchange(true, std::memory_order_acquire);
		}

		Sequence await_resume() noexcept {
			return _last_known_published;
		}

	};

	template<typename Sequence, typename Traits>
	class SequenceBarrierWaitOperation
	: public SequenceBarrierWaitOperationBase<Sequence, Traits> {
		using schedule_operation = EnsureOnExecutor::OnAwaitable;

        EnsureOnExecutor* _executor;
		// Can't use std::optional<T> here since T could be a reference.
		detail::manual_lifetime<schedule_operation> _schedule_operation;
		detail::manual_lifetime<typename detail::awaitable_traits<schedule_operation>::awaiter_t> _schedule_awaiter;
		bool _is_schedule_operation_created = false;
		bool _is_schedule_awaiter_created = false;

	public:
		SequenceBarrierWaitOperation(
			const SequenceBarrier<Sequence, Traits>& barrier,
			Sequence target_sequence,
			EnsureOnExecutor* executor
        ) noexcept
        : SequenceBarrierWaitOperationBase<Sequence, Traits> { barrier, target_sequence }
        , _executor { executor }
		{}

		SequenceBarrierWaitOperation(const SequenceBarrierWaitOperation& other) noexcept
        : SequenceBarrierWaitOperationBase<Sequence, Traits> { other }
        , _executor { other._executor }
		{}

		~SequenceBarrierWaitOperation() {
			if (_is_schedule_awaiter_created) {
				_schedule_awaiter.destruct();
			}
			if (_is_schedule_operation_created) {
				_schedule_operation.destruct();
			}
		}

		decltype(auto) await_resume() noexcept(noexcept(_schedule_awaiter->await_resume())) {
			if (_is_schedule_awaiter_created) {
				_schedule_awaiter->await_resume();
			}

			return SequenceBarrierWaitOperationBase<Sequence, Traits>::await_resume();
		}

	private:

		void resume_impl() noexcept override {
			try {
				_schedule_operation.construct(_executor->ensure_on(YAZ_LOCATION_STR));
				_is_schedule_operation_created = true;

				_schedule_awaiter.construct(detail::get_awaiter(
					static_cast<schedule_operation&&>(*_schedule_operation)
				));
				_is_schedule_awaiter_created = true;

				if (!_schedule_awaiter->await_ready()) {
					using await_suspend_result_t = decltype(_schedule_awaiter->await_suspend(this->_awaiting_coroutine));
					if constexpr (std::is_void_v<await_suspend_result_t>) {
						_schedule_awaiter->await_suspend(this->_awaiting_coroutine);
						return;
					} else if constexpr (std::is_same_v<await_suspend_result_t, bool>) {
						if (_schedule_awaiter->await_suspend(this->_awaiting_coroutine)) {
							return;
						}
					} else {
						// Assume it returns a coroutine_handle.
						_schedule_awaiter->await_suspend(this->_awaiting_coroutine).resume();
						return;
					}
				}
			} catch (...) {
				// Ignore failure to reschedule and resume inline?
				// Should we catch the exception and rethrow from await_resume()?
				// Or should we require that 'co_await scheduler.schedule()' is noexcept?
			}

			// Resume outside the catch-block.
			this->_awaiting_coroutine.resume();
		}
	};

    template<typename Sequence, typename Traits>
	void SequenceBarrier<Sequence, Traits>::publish(Sequence sequence) noexcept {
		_last_published.store(sequence, std::memory_order_seq_cst);

		// Cheaper check to see if there are any awaiting coroutines.
		auto* awaiters = _awaiters.load(std::memory_order_seq_cst);
		if (awaiters == nullptr) {
			return;
		}

		// Acquire the list of awaiters.
		// Note we may be racing with add_awaiter() which could also acquire the list of waiters
		// so we need to check again whether we won the race and acquired the list.
		awaiters = _awaiters.exchange(nullptr, std::memory_order_acquire);
		if (awaiters == nullptr) {
			return;
		}

		// Check the list of awaiters for ones that are now satisfied by the sequence number
		// we just published. Awaiters are added to either the 'awaitersToResume' list or to
		// the 'awaitersToRequeue' list.
		awaiter_t* awaiters_to_resume;
		awaiter_t** awaiters_to_resume_tail = &awaiters_to_resume;

		awaiter_t* awaiters_to_requeue;
		awaiter_t** awaiters_to_requeue_tail = &awaiters_to_requeue;

		do {
			if (Traits::precedes(sequence, awaiters->_target_sequence)) {
				// Target sequence not reached. Append to 'requeue' list.
				*awaiters_to_requeue_tail = awaiters;
				awaiters_to_requeue_tail = &awaiters->_next;
			} else {
				// Target sequence reached. Append to 'resume' list.
				*awaiters_to_resume_tail = awaiters;
				awaiters_to_resume_tail = &awaiters->_next;
			}
			awaiters = awaiters->_next;
		} while (awaiters != nullptr);

		// Null-terminate the two lists.
		*awaiters_to_requeue_tail = nullptr;
		*awaiters_to_resume_tail = nullptr;

		if (awaiters_to_requeue != nullptr) {
			awaiter_t* oldHead = nullptr;
			while (!_awaiters.compare_exchange_weak(
				oldHead,
				awaiters_to_requeue,
				std::memory_order_release,
				std::memory_order_relaxed
            )) {
				*awaiters_to_requeue_tail = oldHead;
			}
		}

		while (awaiters_to_resume != nullptr) {
			auto* next = awaiters_to_resume->_next;
			awaiters_to_resume->_last_known_published = sequence;
			awaiters_to_resume->resume();
			awaiters_to_resume = next;
		}
	}

	template<typename Sequence, typename Traits>
	void SequenceBarrier<Sequence, Traits>::add_awaiter(awaiter_t* awaiter) const noexcept {
		Sequence target_sequence = awaiter->_target_sequence;
		awaiter_t* awaiters_to_requeue = awaiter;
		awaiter_t** awaiters_to_requeue_tail = &awaiter->_next;

		Sequence last_known_published;
		awaiter_t* awaiters_to_resume;
		awaiter_t** awaiters_to_resume_tail = &awaiters_to_resume;

		do {
			// Enqueue the awaiter(s)
			{
				auto* old_head = _awaiters.load(std::memory_order_relaxed);
				do {
					*awaiters_to_requeue_tail = old_head;
				} while (!_awaiters.compare_exchange_weak(
					old_head,
					awaiters_to_requeue,
					std::memory_order_seq_cst,
					std::memory_order_relaxed
                ));
			}

			// Check that the sequence we were waiting for wasn't published while
			// we were enqueueing the waiter.
			// This needs to be seq_cst memory order to ensure that in the case that the producer
			// publishes a new sequence number concurrently with this call that we either see
			// their write to m_lastPublished after enqueueing our awaiter, or they see our
			// write to m_awaiters after their write to m_lastPublished.
			last_known_published = _last_published.load(std::memory_order_seq_cst);
			if (Traits::precedes(last_known_published, target_sequence)) {
				// None of the the awaiters we enqueued have been satisfied yet.
				break;
			}

			// Reset the requeue list to empty
			awaiters_to_requeue_tail = &awaiters_to_requeue;

			// At least one of the awaiters we just enqueued is now satisfied by a concurrently
			// published sequence number. The producer thread may not have seen our write to m_awaiters
			// so we need to try to re-acquire the list of awaiters to ensure that the waiters that
			// are now satisfied are woken up.
			auto* awaiters = _awaiters.exchange(nullptr, std::memory_order_acquire);

			auto min_diff = std::numeric_limits<typename Traits::difference_type>::max();

			while (awaiters != nullptr) {
				const auto diff = Traits::difference(awaiters->_target_sequence, last_known_published);
				if (diff > 0) {
					*awaiters_to_requeue_tail = awaiters;
					awaiters_to_requeue_tail = &awaiters->_next;
					min_diff = diff < min_diff ? diff : min_diff;
				} else {
					*awaiters_to_resume_tail = awaiters;
					awaiters_to_resume_tail = &awaiters->_next;
				}

				awaiters = awaiters->_next;
			}

			// Null-terminate the list of awaiters to requeue.
			*awaiters_to_requeue_tail = nullptr;

			// Calculate the earliest target sequence required by any of the awaiters to requeue.
			target_sequence = static_cast<Sequence>(last_known_published + min_diff);

		} while (awaiters_to_requeue != nullptr);

		// Null-terminate the list of awaiters to resume
		*awaiters_to_resume_tail = nullptr;

		// Resume the awaiters that are ready
		while (awaiters_to_resume != nullptr) {
			auto* next = awaiters_to_resume->_next;
			awaiters_to_resume->_last_known_published = last_known_published;
			awaiters_to_resume->resume();
			awaiters_to_resume = next;
		}
	}

} // end of ::yazik::concurrency namespace

#undef CPPCORO_CPU_CACHE_LINE