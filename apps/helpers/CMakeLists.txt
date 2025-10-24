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


add_subdirectory(e2)
add_subdirectory(f1u)
add_subdirectory(hal)
add_subdirectory(logger)
add_subdirectory(metrics)
add_subdirectory(network)
add_subdirectory(tracing)

add_library(srsran_app_helpers INTERFACE)
target_link_libraries(srsran_app_helpers INTERFACE  srsran_e2_app_helpers
                                                    srsran_f1u_app_helpers
                                                    srsran_hal_app_helpers
                                                    srsran_logger_app_helpers
                                                    srsran_metrics_app_helpers
                                                    srsran_network_app_helpers
                                                    srsran_tracing_app_helpers)
