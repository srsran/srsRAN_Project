#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
            o_du_low_metrics_consumers.cpp
            ru_metrics_consumers.cpp)

add_library(srsran_flexible_o_du_split_helpers_metrics_consumers STATIC ${SOURCES})
target_include_directories(srsran_flexible_o_du_split_helpers_metrics_consumers PRIVATE ${CMAKE_SOURCE_DIR})
