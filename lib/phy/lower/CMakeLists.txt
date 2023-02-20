#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(modulation)
add_subdirectory(processors)
add_subdirectory(amplitude_controller)

add_library(srsran_lower_phy STATIC lower_phy_factory.cpp lower_phy_impl.cpp)
target_link_libraries(srsran_lower_phy
  srsran_generic_funcs
  srsran_lower_phy_adaptors
  srsran_lower_phy_modulation
  srsran_lower_phy_prach_processor
  srsran_lower_phy_amplitude_controller)
