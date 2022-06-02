#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(srsgnb_ran band_helper.cpp ssb_mapping.cpp pdcch/cce_to_prb_mapping.cpp pdcch/dci_packing.cpp)
target_link_libraries(srsgnb_ran)
