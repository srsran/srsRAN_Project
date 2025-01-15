#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

# Do not build by default.
set_directory_properties(PROPERTIES EXCLUDE_FROM_ALL TRUE)

add_subdirectory(buffer_pool)
add_subdirectory(e2)
add_subdirectory(hal)
add_subdirectory(logger)
add_subdirectory(network)
add_subdirectory(worker_manager)

set(SOURCES
        stdin_command_dispatcher.cpp)

add_library(srsran_app_services STATIC ${SOURCES})
target_include_directories(srsran_app_services PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(srsran_app_services
        srsran_buffer_pool_app_service
        srsran_e2_app_service
        srsran_network_app_service
        srsran_hal_app_service
        srsran_logger_app_service
        srsran_worker_manager_app_service)
