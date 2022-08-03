#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(du_example du_example.cpp)
target_link_libraries(du_example
        srsgnb_du_high
        srsgnb_gateway
        mac_fapi_adaptor_factory
        phy_fapi_adaptor_factory
        srsgnb_phy_support
        srsgnb_upper_phy
        srsgnb_downlink_processor
        srsgnb_signal_processors
        srsgnb_channel_processors
        srsgnb_lower_phy
        srsgnb_radio)
target_include_directories(du_example PRIVATE ${CMAKE_SOURCE_DIR})
