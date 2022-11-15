#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "support")

add_subdirectory(io_broker)

add_executable(timer_test timer_test.cpp)
target_link_libraries(timer_test srslog srsgnb_support pthread)
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
target_link_libraries(async_exec_test srslog srsgnb_support)
add_test(async_exec_test async_exec_test)

add_executable(async_queue_test async_queue_test.cpp)
target_link_libraries(async_queue_test srslog srsgnb_support)
add_test(async_queue_test async_queue_test)

add_executable(async_task_sequencer_test async_task_sequencer_test.cpp)
target_link_libraries(async_task_sequencer_test srslog srsgnb_support)
add_test(async_task_sequencer_test async_task_sequencer_test)

add_executable(unique_thread_test unique_thread_test.cpp)
target_link_libraries(unique_thread_test srslog srsgnb_support)
add_test(unique_thread_test unique_thread_test)
set_tests_properties(unique_thread_test PROPERTIES LABELS "tsan")

add_executable(stats_test stats_test.cpp)
target_link_libraries(stats_test srslog)
add_test(stats_test stats_test)

add_executable(bit_encoding_test bit_encoding_test.cpp)
target_link_libraries(bit_encoding_test srslog srsgnb_support)
add_test(bit_encoding_test bit_encoding_test)

add_executable(protocol_transaction_manager_test protocol_transaction_manager_test.cpp)
target_link_libraries(protocol_transaction_manager_test srslog srsgnb_support)
add_test(protocol_transaction_manager_test protocol_transaction_manager_test)

add_executable(ring_buffer_pool_test ring_buffer_pool_test.cpp)
target_link_libraries(ring_buffer_pool_test srsgnb_support gtest gtest_main)
add_test(ring_buffer_pool_test ring_buffer_pool_test)

add_executable(fixed_size_memory_pool_test fixed_size_memory_pool_test.cpp)
target_link_libraries(fixed_size_memory_pool_test srsgnb_support gtest gtest_main)
add_test(fixed_size_memory_pool_test fixed_size_memory_pool_test)