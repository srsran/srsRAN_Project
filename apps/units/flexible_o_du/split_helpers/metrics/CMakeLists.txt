#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(consumers)

set(SOURCES
        flexible_o_du_metrics_builder.cpp
        flexible_o_du_metrics_consumers.cpp
        flexible_o_du_metrics_producer.cpp)

add_library(srsran_flexible_o_du_unit_metrics_helpers STATIC ${SOURCES})
target_include_directories(srsran_flexible_o_du_unit_metrics_helpers PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(srsran_flexible_o_du_unit_metrics_helpers PRIVATE srsran_metrics_json_generators srsran_flexible_o_du_split_helpers_metrics_consumers)
