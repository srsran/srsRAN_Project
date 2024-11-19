#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        o_cu_up_e2_config_cli11_schema.cpp
        o_cu_up_e2_config_translators.cpp
        o_cu_up_e2_config_yaml_writer.cpp)

add_library(srsran_o_cu_up_app_unit_e2 STATIC ${SOURCES})
target_include_directories(srsran_o_cu_up_app_unit_e2 PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(srsran_o_cu_up_app_unit_e2 srsran_e2_app_service)
