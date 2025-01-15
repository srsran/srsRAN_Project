#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

if (DPDK_FOUND)
    add_definitions(-DDPDK_FOUND)
endif (DPDK_FOUND)

set(SOURCES
        ru_ofh_config_cli11_schema.cpp
        ru_ofh_factories.cpp
        ru_ofh_config_translator.cpp
        ru_ofh_config_validator.cpp
        ru_ofh_config_yaml_writer.cpp)

add_library(srsran_split_7_2_app_unit_helpers STATIC ${SOURCES})
target_link_libraries(srsran_split_7_2_app_unit_helpers srsran_ru_ofh srsran_cpu_affinities_helper)
target_include_directories(srsran_split_7_2_app_unit_helpers PRIVATE ${CMAKE_SOURCE_DIR})
