#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(modulation)
add_subdirectory(processors)
add_subdirectory(amplitude_controller)

add_library(srsgnb_lower_phy STATIC lower_phy_factory.cpp lower_phy_impl.cpp)
target_link_libraries(srsgnb_lower_phy
  srsgnb_generic_funcs
  srsgnb_lower_phy_adaptors
  srsgnb_lower_phy_modulation
  srsgnb_lower_phy_prach_processor
  srsgnb_lower_phy_amplitude_controller)
