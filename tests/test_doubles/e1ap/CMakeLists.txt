#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(e1ap_test_doubles e1ap_test_message_validators.cpp)
set_target_properties(e1ap_test_doubles PROPERTIES UNITY_BUILD ON)
target_link_libraries(e1ap_test_doubles srsran_support srslog e1ap_asn1)
