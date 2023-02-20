#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(e1ap_cu_cp_test_helpers e1ap_cu_cp_test_helpers.cpp)
target_include_directories(e1ap_cu_cp_test_helpers PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(e1ap_cu_cp_test_helpers e1ap_test_helpers srsgnb_e1ap srsgnb_support e1ap_asn1 srslog)

set(SOURCES
        e1ap_cu_cp_setup_procedure_test.cpp
        e1ap_cu_cp_bearer_context_setup_procedure_test.cpp
        e1ap_cu_cp_bearer_context_modification_procedure_test.cpp
        e1ap_cu_cp_bearer_context_release_procedure_test.cpp
        e1ap_cu_cp_test.cpp)

add_executable(e1ap_cu_cp_test ${SOURCES})
target_include_directories(e1ap_cu_cp_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(e1ap_cu_cp_test e1ap_cu_cp_test_helpers srsgnb_e1ap srsgnb_support srslog e1ap_asn1 gtest gtest_main)
gtest_discover_tests(e1ap_cu_cp_test PROPERTIES "LABELS;e1ap_cu_cp")
