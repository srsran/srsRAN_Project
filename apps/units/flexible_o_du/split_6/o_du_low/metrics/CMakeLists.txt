#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        split6_flexible_o_du_low_metrics_builder.cpp
        split6_flexible_o_du_low_metrics_consumers.cpp
        split6_flexible_o_du_low_metrics_producer.cpp)

add_library(srsran_split6_flexible_o_du_low_unit_metrics_helpers STATIC ${SOURCES})
target_include_directories(srsran_split6_flexible_o_du_low_unit_metrics_helpers PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(srsran_split6_flexible_o_du_low_unit_metrics_helpers PRIVATE srsran_flexible_o_du_split_helpers_metrics_consumers srsran_app_services)
