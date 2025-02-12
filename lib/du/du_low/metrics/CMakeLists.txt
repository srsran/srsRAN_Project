#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        du_low_ldpc_metrics_collector.cpp
        du_low_pdsch_metrics_collector.cpp
        du_low_pusch_metrics_collector.cpp
        o_du_low_metrics_collector_impl.cpp)

add_library(srsran_o_du_low_metrics STATIC ${SOURCES})
target_link_libraries(srsran_o_du_low_metrics srsran_support)
target_include_directories(srsran_o_du_low_metrics PRIVATE ${CMAKE_SOURCE_DIR})
