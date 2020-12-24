#include <yazik/utility/lambda_continuation.hpp>
#include <stack>

namespace yazik {
namespace utility {

    OnceImpl::OnceImpl(std::once_flag& once_flag): _once_flag{once_flag} {}
    Result<void> OnceImpl::apply(continuation_t&& continuation) {
        Result<void> result = yaz_ok();
        std::call_once(
            _once_flag,
            [&result, _l_move(continuation)] {
                result = continuation();
            }
        );
        return result;
    }

    class ThreadExiter {
        std::stack<std::function<void()>> _exit_funcs;
    public:
        ThreadExiter() = default;
        ThreadExiter(ThreadExiter const&) = delete;
        void operator=(ThreadExiter const&) = delete;
        ~ThreadExiter() {
            while(!_exit_funcs.empty()) {
                _exit_funcs.top()();
                _exit_funcs.pop();
            }
        }
        void add(std::function<void()> func) {
            _exit_funcs.push(std::move(func));
        }
    };


    void OnThreadExitImpl::apply(continuation_t&& continuation) {
        thread_local ThreadExiter s_exiter;
        s_exiter.add(std::move(continuation));
    }

} // end of ::yazik::utility namespace
} // end of ::yazik namespace