#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        cu_up_pdcp_metrics_producer.cpp
        cu_up_pdcp_metrics_consumers.cpp)

add_library(srsran_cu_up_unit_metrics_helpers STATIC ${SOURCES})
target_include_directories(srsran_cu_up_unit_metrics_helpers PRIVATE ${CMAKE_SOURCE_DIR})
