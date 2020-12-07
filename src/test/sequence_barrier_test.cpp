#include <yazik/testlib/yaz_test_macros.hpp>
#include <yazik/utility/utility_defs.hpp>
#include <yazik/concurrency/sequence_barrier.hpp>
#include <yazik/concurrency/scheduler.hpp>

using namespace yazik;
using namespace yazik::concurrency;

Future<std::size_t> wait(
    SequenceBarrier<>& barrier,
    std::size_t desired,
    const executor_ptr_t& executor,
    uint64_t executor_thread_idx
) {
    co_await barrier.wait_until_published(
        desired,
        executor.get()
    );
    std::size_t cur = barrier.last_published();
//    L::debug("sequence got {}", cur);
//    L::debug("thread check: {} - {}", thread_idx(), executor_thread_idx);
    REQUIRE(thread_idx() == executor_thread_idx);
    co_return cur;
}

Future<> sequence_barrier_test(
    const vector<executor_ptr_t>& executors,
    const scheduler_ptr_t& scheduler
) {
    auto barrier = SequenceBarrier<> {};

    auto thread_ids = vector<uint64_t>{};
    for (int i = 0; i < 6; ++i)
        thread_ids.push_back(co_await executors[i]->dispatch_task([]{
            return thread_idx();
        } YAZ_PRE_COMMA_LOCATION_STR));

    co_await scheduler->on(YAZ_LOCATION_STR);

    auto futures = vector<Future<std::size_t>>{};
    for (int i = 0; i < 6; ++i)
        futures.push_back(wait(barrier, i, executors[i], thread_ids[i]));

    for (int i = 0; i < 6; ++i) {
        barrier.publish(i);
        co_await scheduler->schedule_delayed(std::chrono::milliseconds { 1 });
    }

    for (int i = 0; i < 6; ++i) {
        auto cur = co_await futures[i];
//        L::debug("results: {} - {}", i, cur);
        REQUIRE(i == cur);
    }

    co_return;
}

TEST_CASE( "sequence_barrier test", "[concurrency/sequence_barrier]" ) {
    auto executors = vector<executor_ptr_t>{};
    for (int i = 0; i < 6; ++i)
        executors.push_back(create_single_thread_executor());

    auto main_scheduler = create_asio_scheduler();
    main_scheduler->run_until_done(sequence_barrier_test(
        executors,
        main_scheduler
    )).get_unwrapped();
}

TEST_CASE( "sequence_barrier same executor test", "[concurrency/sequence_barrier]" ) {
    auto main_scheduler = create_asio_scheduler();
    auto executors = vector<executor_ptr_t>{};
    for (int i = 0; i < 6; ++i)
        executors.push_back(main_scheduler);

    main_scheduler->run_until_done(sequence_barrier_test(
        executors,
        main_scheduler
    )).get_unwrapped();
}

Future<> test_overflow(const scheduler_ptr_t& scheduler) {
    auto barrier = SequenceBarrier<uint8_t> {};
    barrier.publish(1);
    co_await barrier.wait_until_published(
        0,
        scheduler.get()
    );
    co_await barrier.wait_until_published(
        1,
        scheduler.get()
    );

    barrier.publish(255);
    co_await barrier.wait_until_published(
        255,
        scheduler.get()
    );

    barrier.publish(256);
    co_await barrier.wait_until_published(
        256,
        scheduler.get()
    );
    co_await barrier.wait_until_published(
        0,
        scheduler.get()
    );
}

TEST_CASE( "sequence_barrier overflow test", "[concurrency/sequence_barrier]" ) {
    auto main_scheduler = create_asio_scheduler();
    main_scheduler->run_until_done(test_overflow(main_scheduler)).get_unwrapped();
}