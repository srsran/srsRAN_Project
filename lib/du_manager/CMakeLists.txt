#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(ran_resource_management)
add_subdirectory(du_ue)
add_subdirectory(converters)
add_subdirectory(procedures)

set(SOURCES du_manager_factory.cpp
            du_manager_impl.cpp
            du_ue/du_ue_manager.cpp
            du_cell_manager.cpp)

add_library(srsran_du_manager STATIC ${SOURCES})
target_link_libraries(srsran_du_manager
                      srsran_du_manager_procedures
                      du_resource_manager
                      du_ue
                      du_manager_converters
                      srsran_du_config_validators
                      mac_configuration_helpers
                      srsran_rlc
                      srsran_f1u_du
                      srsran_gtpu)
