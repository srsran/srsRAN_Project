#
# Copyright 2021-2024 Software Radio Systems Limited
#
# This file is part of srsRAN
#
# srsRAN is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsRAN is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#

set_directory_properties(PROPERTIES LABELS "support")

add_executable(task_worker_test sync_task_executor_test.cpp task_worker_test.cpp strand_executor_test.cpp task_execution_manager_test.cpp)
target_link_libraries(task_worker_test srslog srsran_support gtest gtest_main)
add_test(task_worker_test task_worker_test)
set_tests_properties(task_worker_test PROPERTIES LABELS "tsan")

