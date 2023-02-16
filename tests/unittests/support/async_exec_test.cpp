/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/support/async/eager_async_task.h"
#include "srsgnb/support/async/execute_on.h"
#include "srsgnb/support/executors/blocking_task_worker.h"
#include "srsgnb/support/executors/task_worker.h"
#include "srsgnb/support/test_utils.h"

using namespace srsgnb;

/// In this test, we keep hopping between workers. In each worker we increment an unprotected integer, and print
/// Given that the access to the integer is sequential, we dont need mutexes.
void test_move_exec_context()
{
  test_delimit_logger delimiter{"Switch Execution Context"};

  blocking_task_worker worker0{128};
  task_worker          worker1{"WORKER1", 128}, worker2{"WORKER2", 128};
  task_executor&       exec0{worker0};
  task_worker_executor exec1{worker1};
  task_worker_executor exec2{worker2};

  int                   count = 0;
  eager_async_task<int> t =
      launch_async([&exec0, exec1, exec2, &count, &worker0](coro_context<eager_async_task<int>>& ctx) mutable {
        CORO_BEGIN(ctx);
        count++;
        fmt::print("{}: Running in thread: \"{}\"\n", count, this_thread_name());
        CORO_AWAIT(execute_on(exec1));
        count++;
        fmt::print("{}: Running in thread: \"{}\"\n", count, this_thread_name());
        CORO_AWAIT(execute_on(exec2));
        count++;
        fmt::print("{}: Running in thread: \"{}\"\n", count, this_thread_name());
        CORO_AWAIT(defer_to(exec0));
        count++;
        fmt::print("{}: Running in thread: \"{}\"\n", count, this_thread_name());
        worker0.request_stop(); // as we are in worker0, the cancel command is only processed after the return
        CORO_RETURN(count);
      });

  worker0.run();
  TESTASSERT_EQ(4, count);
}

/// In this test, we verify the correctness of the task offloader. An awaitable that runs a task in another
/// execution context and resumes back in the original execution context
void test_offload_exec()
{
  test_delimit_logger delimiter{"Offload Execution Context"};

  blocking_task_worker worker0{128};
  task_worker          worker1{"WORKER1", 128}, worker2{"WORKER2", 128};
  task_executor&       exec0{worker0};
  task_worker_executor exec1{worker1};
  task_worker_executor exec2{worker2};

  int  count     = 0;
  auto inc_count = [&count]() {
    count++;
    fmt::print("{}: Running in thread: \"{}\"\n", count, this_thread_name());
    return count;
  };
  eager_async_task<int> t =
      launch_async([&exec0, exec1, exec2, &inc_count, &worker0](coro_context<eager_async_task<int>>& ctx) mutable {
        CORO_BEGIN(ctx);
        inc_count();
        CORO_AWAIT(offload_to_executor(exec1, exec0, inc_count));
        CORO_AWAIT(offload_to_executor(exec2, exec0, inc_count));
        CORO_AWAIT(offload_to_executor(exec1, exec0, inc_count));
        CORO_AWAIT(offload_to_executor(exec2, exec0, inc_count));
        worker0.request_stop(); // as we are in worker0, the cancel command is only processed after the return
        CORO_RETURN(inc_count());
      });

  worker0.run();
  TESTASSERT_EQ(6, count);
}

int main()
{
  test_move_exec_context();
  test_offload_exec();
}
