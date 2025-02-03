#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        app_resource_usage.cpp)

add_library(srsran_resource_usage_app_service STATIC ${SOURCES})
target_include_directories(srsran_resource_usage_app_service PRIVATE ${CMAKE_SOURCE_DIR})
