#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(converters)
add_subdirectory(du_ue)
add_subdirectory(procedures)

set(SOURCES du_manager_factory.cpp du_manager_impl.cpp du_ue_manager.cpp du_cell_manager.cpp)

add_library(srsgnb_du_manager STATIC ${SOURCES})
target_link_libraries(srsgnb_du_manager
                      srsgnb_du_manager_procedures
                      du_manager_converters
                      du_manager_ue
                      srsgnb_du_config_validators
                      srsgnb_rlc)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
