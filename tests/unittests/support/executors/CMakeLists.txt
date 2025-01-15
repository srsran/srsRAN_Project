#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "support")

add_executable(task_worker_test sync_task_executor_test.cpp task_worker_test.cpp strand_executor_test.cpp task_execution_manager_test.cpp)
target_link_libraries(task_worker_test srslog srsran_support gtest gtest_main)
gtest_discover_tests(task_worker_test)
add_test(task_worker_test task_worker_test)
set_tests_properties(task_worker_test PROPERTIES LABELS "tsan")

