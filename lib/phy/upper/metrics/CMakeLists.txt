#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        upper_phy_ldpc_metrics_collector.cpp
        upper_phy_pdsch_metrics_collector.cpp
        upper_phy_pusch_metrics_collector.cpp
        upper_phy_metrics_collector_impl.cpp)

add_library(srsran_upper_phy_metrics STATIC ${SOURCES})
target_link_libraries(srsran_upper_phy_metrics srsran_support)
target_include_directories(srsran_upper_phy_metrics PRIVATE ${CMAKE_SOURCE_DIR})
