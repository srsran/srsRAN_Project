#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "support")

add_subdirectory(io_broker)

add_executable(task_worker_test task_worker_test.cpp)
target_link_libraries(task_worker_test srslog srsran_support gtest gtest_main)
gtest_discover_tests(task_worker_test)

add_executable(timer_test timer_test.cpp)
target_link_libraries(timer_test srslog srsran_support pthread)
add_test(timer_test timer_test)

add_executable(function_signature_test function_signature_test.cpp)
add_test(function_signature_test function_signature_test)

add_executable(async_event_test async_event_test.cpp)
target_link_libraries(async_event_test srslog)
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

add_executable(async_task_sequencer_test async_task_sequencer_test.cpp)
target_link_libraries(async_task_sequencer_test srslog srsran_support)
add_test(async_task_sequencer_test async_task_sequencer_test)

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

add_executable(ring_buffer_pool_test ring_buffer_pool_test.cpp)
target_link_libraries(ring_buffer_pool_test srsran_support gtest gtest_main)
add_test(ring_buffer_pool_test ring_buffer_pool_test)

add_executable(fixed_size_memory_pool_test fixed_size_memory_pool_test.cpp)
target_link_libraries(fixed_size_memory_pool_test srsran_support gtest gtest_main)
add_test(fixed_size_memory_pool_test fixed_size_memory_pool_test)

add_executable(units_test units_test.cpp)
target_link_libraries(units_test srslog srsran_support gtest gtest_main)
add_test(units_test units_test)

add_executable(backtrace_test backtrace_test.cpp)
target_link_libraries(backtrace_test srsran_support)
add_backward(backtrace_test)

add_executable(validator_helpers_test config/validator_helpers_test.cpp)
target_link_libraries(validator_helpers_test srsran_support gtest gtest_main)
add_test(validator_helpers_test validator_helpers_test)
