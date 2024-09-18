#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(ngap_test_doubles ngap_test_message_validators.cpp)
target_link_libraries(ngap_test_doubles srsran_support srslog ngap_asn1)

add_to_exported_libs(ngap_test_doubles)
