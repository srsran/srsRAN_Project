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

add_subdirectory(network)
add_subdirectory(executors)

add_executable(timer_test timer_test.cpp)
target_link_libraries(timer_test srslog srsran_support gtest gtest_main)
add_test(timer_test timer_test)
set_tests_properties(timer_test PROPERTIES LABELS "tsan")

add_executable(function_signature_test function_signature_test.cpp)
add_test(function_signature_test function_signature_test)

add_executable(async_event_test async_event_test.cpp)
target_link_libraries(async_event_test srslog gtest gtest_main)
add_test(async_event_test async_event_test)

add_executable(async_task_test async_task_test.cpp)
target_link_libraries(async_task_test srslog)
add_test(async_task_test async_task_test)

add_executable(async_exec_test async_exec_test.cpp)
target_link_libraries(async_exec_test srslog srsran_support)
add_test(async_exec_test async_exec_test)

add_executable(async_queue_test async_queue_test.cpp)
target_link_libraries(async_queue_test srslog srsran_support gtest gtest_main)
add_test(async_queue_test async_queue_test)

add_executable(fifo_async_task_scheduler_test fifo_async_task_scheduler_test.cpp)
target_link_libraries(fifo_async_task_scheduler_test srslog srsran_support)
add_test(fifo_async_task_scheduler_test fifo_async_task_scheduler_test)

add_executable(unique_thread_test unique_thread_test.cpp)
target_link_libraries(unique_thread_test srslog srsran_support)
add_test(unique_thread_test unique_thread_test)
set_tests_properties(unique_thread_test PROPERTIES LABELS "tsan")

add_executable(stats_test stats_test.cpp)
target_link_libraries(stats_test srslog)
add_test(stats_test stats_test)

add_executable(bit_encoding_test bit_encoding_test.cpp)
target_link_libraries(bit_encoding_test srslog srsran_support)
add_test(bit_encoding_test bit_encoding_test)

add_executable(async_event_observer_test async_event_observer_test.cpp protocol_transaction_manager_test.cpp)
target_link_libraries(async_event_observer_test srslog srsran_support gtest gtest_main)
gtest_discover_tests(async_event_observer_test)

add_executable(memory_block_list_test memory_block_list_test.cpp)
target_link_libraries(memory_block_list_test srsran_support gtest gtest_main)
add_test(memory_block_list_test memory_block_list_test)

add_executable(ring_buffer_pool_test ring_buffer_pool_test.cpp)
target_link_libraries(ring_buffer_pool_test srsran_support gtest gtest_main)
add_test(ring_buffer_pool_test ring_buffer_pool_test)

add_executable(fixed_size_memory_pool_test fixed_size_memory_pool_test.cpp)
target_link_libraries(fixed_size_memory_pool_test srsran_support gtest gtest_main)
add_test(fixed_size_memory_pool_test fixed_size_memory_pool_test)
set_tests_properties(fixed_size_memory_pool_test PROPERTIES LABELS "tsan")

add_executable(units_test units_test.cpp)
target_link_libraries(units_test srslog srsran_support gtest gtest_main)
add_test(units_test units_test)

add_executable(backtrace_test backtrace_test.cpp)
target_link_libraries(backtrace_test srsran_support)
add_backward(backtrace_test)

add_executable(validator_helpers_test config/validator_helpers_test.cpp)
target_link_libraries(validator_helpers_test srsran_support gtest gtest_main)
add_test(validator_helpers_test validator_helpers_test)

add_executable(event_tracer_test event_tracer_test.cpp)
target_link_libraries(event_tracer_test srsran_support gtest gtest_main)
add_test(event_tracer_test event_tracer_test)

add_executable(complex_normal_random_test complex_normal_random_test.cpp)
target_link_libraries(complex_normal_random_test srslog gtest gtest_main)
add_test(complex_normal_random_test complex_normal_random_test)
