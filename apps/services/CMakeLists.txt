#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

# Do not build by default.
set_directory_properties(PROPERTIES EXCLUDE_FROM_ALL TRUE)

add_subdirectory(app_resource_usage)
add_subdirectory(buffer_pool)
add_subdirectory(cmdline)
add_subdirectory(metrics)
add_subdirectory(remote_control)
add_subdirectory(worker_manager)

add_library(srsran_app_services INTERFACE)
target_link_libraries(srsran_app_services INTERFACE
        srsran_buffer_pool_app_service
        srsran_cmdline_app_service
        srsran_metrics_app_service
        srsran_remote_control_app_service
        srsran_resource_usage_app_service
        srsran_worker_manager_app_service)
