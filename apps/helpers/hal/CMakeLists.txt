#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        hal_cli11_schema.cpp)

add_library(srsran_hal_app_helpers STATIC ${SOURCES})
target_include_directories(srsran_hal_app_helpers PRIVATE ${CMAKE_SOURCE_DIR})
