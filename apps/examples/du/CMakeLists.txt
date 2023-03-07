#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

if (ZEROMQ_FOUND OR UHD_FOUND)
        add_executable(du_example
                du_example.cpp
                fapi_factory.cpp
                phy_factory.cpp
                radio_factory.cpp)

        target_link_libraries(du_example
                srsran_du_high
                srsran_gateway
                mac_fapi_adaptor_factory
                phy_fapi_adaptor_factory
                srsran_fapi_message_loggers
                srsran_phy_support
                srsran_upper_phy
                srsran_signal_processors
                srsran_channel_processors
                srsran_channel_equalizer
                srsran_lower_phy
                srsran_radio)

        target_include_directories(du_example PRIVATE ${CMAKE_SOURCE_DIR})
endif (ZEROMQ_FOUND OR UHD_FOUND)
