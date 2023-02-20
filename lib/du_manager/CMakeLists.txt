#
# Copyright 2021-2023 Software Radio Systems Limited
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
                      du_resource_manager
                      du_ue
                      srsran_du_manager_procedures
                      du_manager_converters
                      srsran_du_config_validators
                      srsran_rlc)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
