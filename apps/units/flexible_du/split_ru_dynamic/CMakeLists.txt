#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        dynamic_du_factory.cpp)

add_library(srsran_flexible_du_ru_dynamic STATIC ${SOURCES})
target_link_libraries(srsran_flexible_du_ru_dynamic
        srsran_du_high
        srsran_du
        mac_fapi_adaptor_factory
        phy_fapi_adaptor_factory
        srsran_fapi
        srsran_phy_support
        srsran_lower_phy
        srsran_upper_phy
        srsran_signal_processors
        srsran_channel_processors
        srsran_channel_equalizer
        srsran_channel_precoder
        srsran_ru_dummy
        srsran_ru_generic
        srsran_ru_ofh
        srsran_pcap
        srsran_app_services)
