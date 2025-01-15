#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(f1u_cu_up_bearer_test f1u_cu_up_bearer_test.cpp)
target_link_libraries(f1u_cu_up_bearer_test srsran_f1u_cu_up srsran_support srslog gtest gtest_main)
target_include_directories(f1u_cu_up_bearer_test PRIVATE ${CMAKE_SOURCE_DIR})
add_test(f1u_cu_up_bearer_test f1u_cu_up_bearer_test)
gtest_discover_tests(f1u_cu_up_bearer_test)

add_executable(f1u_cu_up_session_manager_test f1u_cu_up_session_manager_test.cpp)
target_link_libraries(f1u_cu_up_session_manager_test srsran_f1u_cu_up_split_connector srsran_support srslog gtest gtest_main)
target_include_directories(f1u_cu_up_session_manager_test PRIVATE ${CMAKE_SOURCE_DIR})
add_test(f1u_cu_up_session_manager_test f1u_cu_up_session_manager_test)
gtest_discover_tests(f1u_cu_up_session_manager_test)
