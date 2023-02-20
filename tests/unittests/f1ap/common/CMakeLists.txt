#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(f1ap_test_helpers f1ap_cu_test_messages.cpp f1ap_du_test_messages.cpp)
target_include_directories(f1ap_test_helpers PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(f1ap_test_helpers srsgnb_support f1ap_asn1 srslog)

add_executable(f1ap_asn1_packer_test f1ap_asn1_packer_test.cpp)
target_link_libraries(f1ap_asn1_packer_test
        f1ap_test_helpers
        srsgnb_f1ap_common
        srsgnb_support
        srslog
        f1ap_asn1
        srsgnb_ran
        gtest
        gtest_main)
target_include_directories(f1ap_asn1_packer_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(f1ap_asn1_packer_test)
