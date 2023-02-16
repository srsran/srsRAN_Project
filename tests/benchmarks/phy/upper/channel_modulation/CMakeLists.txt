#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(modulation_chain_benchmark modulation_chain_benchmark.cpp)
target_link_libraries(modulation_chain_benchmark srsgnb_channel_modulation srsvec srslog)
add_test(modulation_chain_benchmark modulation_chain_benchmark -s -R 1)
