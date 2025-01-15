#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        du_high_rlc_metrics_producer.cpp
        du_high_rlc_metrics_consumers.cpp
        du_high_scheduler_cell_metrics_producer.cpp
        du_high_scheduler_cell_metrics_consumers.cpp)

add_library(srsran_du_high_unit_metrics_helpers STATIC ${SOURCES})
target_include_directories(srsran_du_high_unit_metrics_helpers PRIVATE ${CMAKE_SOURCE_DIR})
