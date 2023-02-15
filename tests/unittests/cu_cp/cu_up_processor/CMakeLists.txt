#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(cu_up_processor_test_helpers cu_up_processor_test_helpers.cpp cu_up_processor_routine_manager_test_helpers.cpp)
target_include_directories(cu_up_processor_test_helpers PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(cu_up_processor_test_helpers cu_cp_test_helpers srsgnb_cu_cp srsgnb_e1ap e1ap_test_helpers f1ap_test_helpers rrc_ue_test_helpers srsgnb_support f1ap_asn1 srslog)

set(SOURCES
        cu_up_processor_test.cpp
        initial_cu_up_processor_setup_routine_test.cpp)

add_executable(cu_up_processor_test ${SOURCES})
target_include_directories(cu_up_processor_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(cu_up_processor_test cu_up_processor_test_helpers srsgnb_support srslog gtest gtest_main)
gtest_discover_tests(cu_up_processor_test PROPERTIES "LABELS;cu_up_processor")
