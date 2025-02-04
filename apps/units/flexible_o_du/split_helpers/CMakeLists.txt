#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(metrics)

set(SOURCES
        flexible_o_du_factory.cpp
        flexible_o_du_impl.cpp)

add_library(srsran_flexible_du_helpers STATIC ${SOURCES})
target_include_directories(srsran_flexible_du_helpers PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(srsran_flexible_du_helpers
        srsran_o_du
        srsran_pcap
        srsran_app_services
        srsran_o_du_low_unit_helpers
        srsran_o_du_high_unit_helpers
        srsran_o_du_unit_metrics_helpers)
