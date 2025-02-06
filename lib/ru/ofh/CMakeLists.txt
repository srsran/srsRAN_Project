#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        ru_ofh_config_validator.cpp
        ru_ofh_controller_impl.cpp
        ru_ofh_downlink_plane_handler_proxy.cpp
        ru_ofh_error_handler_impl.cpp
        ru_ofh_factory.cpp
        ru_ofh_impl.cpp
        ru_ofh_metrics_collector_impl.cpp
        ru_ofh_rx_symbol_handler_impl.cpp
        ru_ofh_timing_notifier_impl.cpp
        ru_ofh_uplink_plane_handler_proxy.cpp)

add_library(srsran_ru_ofh STATIC ${SOURCES})
target_link_libraries(srsran_ru_ofh srsran_ofh srsran_ofh_ethernet srsran_ofh_ecpri)
