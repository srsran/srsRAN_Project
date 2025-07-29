#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        ofh.cpp)

add_library(srsran_ru_metrics_json_generators_helpers STATIC ${SOURCES})
target_include_directories(srsran_ru_metrics_json_generators_helpers PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/apps/helpers/metrics)
