#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#


add_subdirectory(helpers)

# Build the flexible du split 8 library when it is selected in the definition.
set(SOURCES
        split_8_o_du_application_unit_impl.cpp
        split_8_o_du_factory.cpp
        split_8_o_du_unit_cli11_schema.cpp
        split_8_o_du_unit_config_validator.cpp
        split_8_o_du_unit_config_yaml_writer.cpp)

add_library(srsran_flexible_o_du_split_8 STATIC ${SOURCES})
target_include_directories(srsran_flexible_o_du_split_8 PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(srsran_flexible_o_du_split_8
        srsran_split_8_app_unit_helpers
        srsran_flexible_du_helpers)
