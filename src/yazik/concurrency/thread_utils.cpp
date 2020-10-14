#include "thread_utils.hpp"
#include <yazik/utility/logging.hpp>

#include <pthread.h>
#include <sched.h>

namespace yazik::concurrency {

    void maximize_thread_priority() {
        pthread_t this_thread = pthread_self();
        int policy = SCHED_OTHER;
        struct sched_param params;
        pthread_getschedparam(this_thread, &policy, &params);
        params.sched_priority = sched_get_priority_max(policy);
        pthread_setschedparam(this_thread, policy, &params);
    }

} // end of yazik::concurrency namespace