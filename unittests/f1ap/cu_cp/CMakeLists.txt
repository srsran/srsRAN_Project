#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(f1_cu_test_helpers f1ap_cu_test_helpers.cpp)
target_include_directories(f1_cu_test_helpers PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(f1_cu_test_helpers f1ap_test_helpers srsgnb_f1c_du f1ap_asn1 srsgnb_support srslog)

set(SOURCES
        f1ap_cu_ue_context_setup_procedure_test.cpp
        f1ap_cu_ue_context_modification_procedure_test.cpp
        f1ap_cu_ue_context_release_procedure_test.cpp
        f1ap_cu_test.cpp
        f1ap_cu_msg_filler_test.cpp)

add_executable(f1ap_cu_test ${SOURCES})
target_include_directories(f1ap_cu_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(f1ap_cu_test f1_cu_test_helpers srsgnb_f1c_cu srslog f1ap_asn1 srsgnb_support gtest gtest_main)
gtest_discover_tests(f1ap_cu_test PROPERTIES "LABELS;f1ap_cu")
