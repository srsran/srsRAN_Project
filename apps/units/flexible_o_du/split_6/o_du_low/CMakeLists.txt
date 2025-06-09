#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(metrics)

set(SOURCES
            cell_operation_request_impl.cpp
            split6_flexible_o_du_low_factory.cpp
            split6_flexible_o_du_low_impl.cpp
            split6_flexible_o_du_low_manager.cpp
            split6_o_du_low_application_unit_impl.cpp
            split6_o_du_low_metrics_collector_impl.cpp
            split6_o_du_low_unit_cli11_schema.cpp
            split6_o_du_low_unit_config_validator.cpp)

add_library(srsran_split6_o_du_low_plugin STATIC split6_o_du_low_plugin_dummy.cpp
                                                 split6_cell_configurator_plugin_dummy.cpp
                                                 split6_slot_configurator_plugin_dummy.cpp)

add_library(srsran_flexible_o_du_low_split_6 STATIC ${SOURCES})
target_include_directories(srsran_flexible_o_du_low_split_6 PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(srsran_flexible_o_du_low_split_6 PRIVATE
        srsran_split6_flexible_o_du_low_unit_metrics_helpers
        srsran_fapi_config_gateway
        srsran_o_du_low_unit_helpers
        srsran_split_8_app_unit_helpers
        srsran_split_7_2_app_unit_helpers
        srsran_split6_o_du_low_plugin)
