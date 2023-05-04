#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(pdxch)

add_library(srsran_lower_phy_downlink_processor STATIC
        downlink_processor_baseband_impl.cpp
        downlink_processor_factories.cpp
        downlink_processor_impl.cpp)