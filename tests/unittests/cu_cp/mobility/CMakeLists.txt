#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(mobility_test_helpers mobility_test_helpers.cpp ../du_processor_test_messages.cpp ../test_helpers.cpp)
target_include_directories(mobility_test_helpers PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(mobility_test_helpers srsran_cu_cp e1ap_test_helpers f1ap_test_helpers rrc_ue_test_helpers ngap_test_helpers srsran_support f1ap_asn1 srslog)

set(SOURCES inter_cu_handover_routine_test.cpp
            handover_reconfiguration_routine_test.cpp)

add_executable(cu_cp_mobility_test ${SOURCES})
target_include_directories(cu_cp_mobility_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(cu_cp_mobility_test cu_cp_test_helpers mobility_test_helpers f1ap_test_doubles srsran_support srslog gtest gtest_main)
gtest_discover_tests(cu_cp_mobility_test PROPERTIES "LABELS;mobility")
