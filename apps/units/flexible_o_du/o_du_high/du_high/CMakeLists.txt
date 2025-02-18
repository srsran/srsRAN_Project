#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(metrics)

set(SOURCES
        du_high_config_cli11_schema.cpp
        du_high_config_translators.cpp
        du_high_config_validator.cpp
        du_high_config_yaml_writer.cpp
        commands/du_high_remote_commands.cpp)

add_library(srsran_du_high_unit_helpers STATIC ${SOURCES})
target_include_directories(srsran_du_high_unit_helpers PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(srsran_du_high_unit_helpers
        sched_config
        srsran_cpu_affinities_helper
        srsran_du_high_unit_metrics_helpers
        srsran_metrics_app_helpers
        srsran_pusch_processor)
