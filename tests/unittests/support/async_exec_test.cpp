/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "srsran/support/async/eager_async_task.h"
#include "srsran/support/async/execute_on.h"
#include "srsran/support/executors/blocking_task_worker.h"
#include "srsran/support/executors/task_worker.h"
#include "srsran/support/test_utils.h"

using namespace srsran;

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
