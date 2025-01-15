#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        dynamic_o_du_application_unit_impl.cpp
        dynamic_o_du_factory.cpp
        dynamic_o_du_translators.cpp
        dynamic_o_du_unit_cli11_schema.cpp
        dynamic_o_du_unit_config_validator.cpp
        dynamic_o_du_unit_config_yaml_writer.cpp)

add_library(srsran_flexible_o_du_split_dynamic STATIC ${SOURCES})
target_include_directories(srsran_flexible_o_du_split_dynamic PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(srsran_flexible_o_du_split_dynamic
        srsran_ru_dummy
        srsran_split_8_app_unit_helpers
        srsran_split_7_2_app_unit_helpers
        srsran_flexible_du_helpers)
