#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(srsran_lower_phy_amplitude_controller STATIC
        amplitude_controller_clipping_impl.cpp
        amplitude_controller_factories.cpp)
target_link_libraries(srsran_lower_phy_amplitude_controller srsvec)
