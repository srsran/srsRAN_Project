#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(pdcp_test_doubles pdcp_pdu_generator.cpp)
set_target_properties(pdcp_test_doubles PROPERTIES UNITY_BUILD ON)
target_link_libraries(pdcp_test_doubles srsran_support srslog)

add_to_exported_libs(pdcp_test_doubles)
