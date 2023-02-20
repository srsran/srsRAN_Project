#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(srsgnb_lower_phy_prach_processor STATIC
  prach_processor_factories.cpp
  prach_processor_worker.cpp)
target_link_libraries(srsgnb_lower_phy_prach_processor srsgnb_lower_phy_modulation)