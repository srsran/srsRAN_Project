#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        ru_sdr_config_cli11_schema.cpp
        ru_sdr_config_translator.cpp
        ru_sdr_config_validator.cpp
        ru_sdr_config_yaml_writer.cpp
        ru_sdr_factories.cpp)

add_library(srsran_split_8_app_unit_helpers STATIC ${SOURCES})
target_link_libraries(srsran_split_8_app_unit_helpers srsran_ru_generic srsran_lower_phy srsran_flexible_du_support)
target_include_directories(srsran_split_8_app_unit_helpers PRIVATE ${CMAKE_SOURCE_DIR})
