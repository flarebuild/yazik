#include "trace_memory_allocations.hpp"

#ifdef YAZ_TEST

#include <sstream>

#include <folly/AtomicUnorderedMap.h>
#include <folly/Demangle.h>

#include <yazik/utility/logging.hpp>

namespace yazik::utility::debugging {

    ::folly::AtomicUnorderedInsertMap<std::atomic_size_t*, string> s_counter_to_name { 1'024 };

    void print_traced() {
        std::stringstream ss;
        ss << std::endl;
        ss << "+++++++++++++ traced allocations: ++++++++++++++" << std::endl;
        for (auto it = s_counter_to_name.cbegin(); it != s_counter_to_name.cend(); ++it)
            ss << it->second << ": " << it->first->load() << std::endl;
        ss << "++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
        L::debug(ss.str());
    }

    void* traced_new(std::atomic_size_t* counter, const std::type_info& type, size_t size) {
        void *ptr = std::malloc(size);
        if (!ptr)
            throw std::bad_alloc{};
        s_counter_to_name.findOrConstruct(
            counter,
            [&] (void* raw) {
                new (raw) string { ::folly::demangle(type) };
            }
        );
        counter->fetch_add(1, std::memory_order_relaxed);
        return ptr;
    }

    void traced_delete(std::atomic_size_t* counter, void* ptr) {
        counter->fetch_sub(1, std::memory_order_relaxed);
        std::free(ptr);
    }

} // end of yazik::utility::debugging namespace

#endif