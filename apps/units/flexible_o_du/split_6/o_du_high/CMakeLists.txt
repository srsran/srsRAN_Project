#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        split6_o_du_application_unit_impl.cpp
        split6_o_du_factory.cpp
        split6_o_du_impl.cpp
        split6_o_du_unit_cli11_schema.cpp
        split6_o_du_unit_config_validator.cpp)

add_library(srsran_split6_plugin STATIC split6_plugin_dummy.cpp)

add_library(srsran_flexible_o_du_split_6 STATIC ${SOURCES})
target_include_directories(srsran_flexible_o_du_split_6 PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(srsran_flexible_o_du_split_6
        srsran_o_du
        srsran_o_du_high_unit_helpers
        srsran_split6_plugin)
