#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "du_high;integrationtest")

add_library(du_high_test_bench test_utils/du_high_env_simulator.cpp)
target_link_libraries(du_high_test_bench srsran_du_high mac_test_doubles f1ap_test_doubles)
target_include_directories(du_high_test_bench PRIVATE ${CMAKE_SOURCE_DIR})

add_executable(du_high_test du_high_test.cpp du_high_many_ues_test.cpp du_high_many_cells_test.cpp)
target_link_libraries(du_high_test
        du_high_test_bench
        f1ap_du_test_helpers
        srsran_gateway
        gtest
        gtest_main
        pdcp_test_doubles)
target_include_directories(du_high_test PRIVATE ${CMAKE_SOURCE_DIR})
add_test(du_high_test du_high_test)
set_tests_properties(du_high_test PROPERTIES LABELS "tsan")

add_executable(paging_du_high_test paging_test.cpp)
target_link_libraries(paging_du_high_test du_high_test_bench f1ap_du_test_helpers srsran_gateway gtest gtest_main)
target_include_directories(paging_du_high_test PRIVATE ${CMAKE_SOURCE_DIR})
add_test(paging_du_high_test paging_du_high_test)
set_tests_properties(paging_du_high_test PROPERTIES LABELS "tsan")

add_executable(mac_test_mode_adapter_test mac_test_mode_adapter_test.cpp)
target_link_libraries(mac_test_mode_adapter_test srsran_du_high gtest gtest_main)
target_include_directories(mac_test_mode_adapter_test PRIVATE ${CMAKE_SOURCE_DIR})
add_test(mac_test_mode_adapter_test mac_test_mode_adapter_test)
