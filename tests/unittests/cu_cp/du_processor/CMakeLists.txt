#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(du_processor_test_helpers du_processor_test_helpers.cpp du_processor_routine_manager_test_helpers.cpp)
target_include_directories(du_processor_test_helpers PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(du_processor_test_helpers cu_cp_test_helpers srsran_cu_cp srsran_e1ap e1ap_test_helpers f1ap_test_helpers rrc_ue_test_helpers srsran_support f1ap_asn1 srslog)

set(SOURCES
        du_processor_test.cpp
        pdu_session_resource_setup_routine_test.cpp)

add_executable(du_processor_test ${SOURCES})
target_include_directories(du_processor_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(du_processor_test du_processor_test_helpers srsran_support srslog gtest gtest_main)
gtest_discover_tests(du_processor_test PROPERTIES "LABELS;du_processor")
