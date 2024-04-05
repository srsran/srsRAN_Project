#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(f1ap_test_doubles f1ap_test_message_validators.cpp f1ap_test_messages.cpp)
set_target_properties(f1ap_test_doubles PROPERTIES UNITY_BUILD ON)
target_link_libraries(f1ap_test_doubles srsran_support srslog f1ap_asn1)
