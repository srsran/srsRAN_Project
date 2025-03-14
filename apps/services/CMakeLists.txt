#
# Copyright 2021-2025 Software Radio Systems Limited
#
# This file is part of srsRAN
#
# srsRAN is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsRAN is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#

# Do not build by default.
set_directory_properties(PROPERTIES EXCLUDE_FROM_ALL TRUE)

add_subdirectory(app_resource_usage)
add_subdirectory(buffer_pool)
add_subdirectory(cmdline)
add_subdirectory(remote_control)
add_subdirectory(worker_manager)

add_library(srsran_app_services INTERFACE)
target_link_libraries(srsran_app_services INTERFACE
        srsran_buffer_pool_app_service
        srsran_cmdline_app_service
        srsran_remote_control_app_service
        srsran_resource_usage_app_service
        srsran_worker_manager_app_service)
