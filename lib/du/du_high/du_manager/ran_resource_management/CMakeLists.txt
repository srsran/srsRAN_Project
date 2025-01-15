#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(du_resource_manager
        du_bearer_resource_manager.cpp
        du_pucch_resource_manager.cpp
        du_ran_resource_manager_impl.cpp
        du_srs_resource_manager.cpp
        pucch_resource_generator.cpp
        srs_resource_generator.cpp
        ue_capability_manager.cpp
        du_drx_resource_manager.cpp
        du_meas_config_manager.cpp)
target_link_libraries(du_resource_manager du_manager_converters srsran_du_config_validators mac_configuration_helpers)
