#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        lower_phy/lower_phy_sector.cpp
        lower_phy/low_phy_sector_metrics_collector.cpp
        lower_phy/low_phy_sector_timing_handler.cpp
        lower_phy/lower_phy_factory.cpp
        ru_controller_generic_impl.cpp
        ru_downlink_handler_generic_impl.cpp
        ru_factory_generic_impl.cpp
        ru_generic_impl.cpp
        ru_metrics_collector_generic_impl.cpp
        ru_uplink_request_handler_generic_impl.cpp)

add_library(srsran_ru_generic STATIC ${SOURCES})
target_link_libraries(srsran_ru_generic srsran_lower_phy srsran_radio srslog)
