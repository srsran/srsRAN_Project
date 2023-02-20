#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(cu_cp_ue_manager_test_helpers ue_manager_test_helpers.cpp)
target_include_directories(cu_cp_ue_manager_test_helpers PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(cu_cp_ue_manager_test_helpers cu_cp_test_helpers srsran_cu_cp srsran_e1ap rrc_ue_test_helpers ngap_test_helpers srsran_support srslog)

set(SOURCES
        ue_manager_test.cpp)

add_executable(cu_cp_ue_manager_test ${SOURCES})
target_include_directories(cu_cp_ue_manager_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(cu_cp_ue_manager_test cu_cp_ue_manager_test_helpers srsran_support srslog gtest gtest_main)
gtest_discover_tests(cu_cp_ue_manager_test PROPERTIES "LABELS;cu_cp_ue_manager")
