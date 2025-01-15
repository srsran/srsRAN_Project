#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

include_directories(../../..)

add_library(rrc_test_doubles rrc_test_messages.cpp rrc_test_message_validators.cpp rrc_packed_test_messages.cpp)
set_target_properties(rrc_test_doubles PROPERTIES UNITY_BUILD ON)
target_link_libraries(rrc_test_doubles rrc_nr_asn1 srsran_rrc srsran_support srslog)
