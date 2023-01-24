#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(cu_cp_test_helpers du_processor_test_messages.cpp test_helpers.cpp)
target_include_directories(cu_cp_test_helpers PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(cu_cp_test_helpers srsgnb_cu_cp srsgnb_support srslog f1ap_test_helpers e1ap_test_helpers f1ap_asn1 ngap_asn1 e1ap_asn1)
