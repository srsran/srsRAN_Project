#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        cli11_cpu_affinities_parser_helper.cpp)

add_library(srsran_flexible_du_helpers STATIC ${SOURCES})
target_include_directories(srsran_flexible_du_helpers PRIVATE ${CMAKE_SOURCE_DIR})
