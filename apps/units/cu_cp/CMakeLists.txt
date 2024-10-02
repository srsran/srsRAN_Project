#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        cu_cp_application_unit_impl.cpp
        cu_cp_builder.cpp
        cu_cp_unit_config_cli11_schema.cpp
        cu_cp_config_translators.cpp
        cu_cp_unit_config_validator.cpp
        cu_cp_unit_config_yaml_writer.cpp
        cu_cp_wrapper.cpp)

add_library(srsran_cu_cp_app_unit STATIC ${SOURCES})
target_include_directories(srsran_cu_cp_app_unit PRIVATE ${CMAKE_SOURCE_DIR})
