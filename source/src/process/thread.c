#include <process/thread.h>

__NORETURN void thread_resume(thread_t * thread) {
    switch (thread->level) {
        case TL_KERNEL: {

        } break;
    }
}