#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(e1ap_test_helpers e1ap_cu_cp_test_messages.cpp e1ap_cu_up_test_messages.cpp)
target_include_directories(e1ap_test_helpers PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(e1ap_test_helpers srsgnb_support e1ap_asn1 srslog)

add_executable(e1ap_asn1_packer_test e1ap_asn1_packer_test.cpp)
target_link_libraries(e1ap_asn1_packer_test
                      e1ap_test_helpers
                      srsgnb_e1ap
                      srsgnb_support
                      srslog
                      e1ap_asn1
                      srsgnb_ran
                      gtest
                      gtest_main)
target_include_directories(e1ap_asn1_packer_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(e1ap_asn1_packer_test)
