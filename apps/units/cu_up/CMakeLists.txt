#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        cu_up_application_unit_impl.cpp
        cu_up_unit_config_validator.cpp
        cu_up_unit_config_cli11_schema.cpp)

add_library(srsran_cu_up_app_unit STATIC ${SOURCES})
target_include_directories(srsran_cu_up_app_unit PRIVATE ${CMAKE_SOURCE_DIR})
