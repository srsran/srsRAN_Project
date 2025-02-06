#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES serving_cell_config_factory.cpp
        sched_cell_config_helpers.cpp
        scheduler_cell_config_validator.cpp
        scheduler_ue_config_validator.cpp
        serving_cell_config_validator.cpp
        csi_helper.cpp
        pucch_resource_generator.cpp)
add_library(sched_config STATIC ${SOURCES})
target_link_libraries(sched_config srsran_ran du_resource_manager)

add_library(sched_config_manager OBJECT
        cell_configuration.cpp
        ue_configuration.cpp
        sched_config_manager.cpp
        du_cell_group_config_pool.cpp
)
target_link_libraries(sched_config_manager srsran_ran)
