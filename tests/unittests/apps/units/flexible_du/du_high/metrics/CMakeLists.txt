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

add_executable(du_high_scheduler_cell_metrics_consumer_stdout_test du_high_scheduler_cell_metrics_consumer_stdout_test.cpp)
target_link_libraries(du_high_scheduler_cell_metrics_consumer_stdout_test srsran_support srsran_du_high_unit_metrics_helpers srsran_app_services)
add_test(du_high_scheduler_cell_metrics_consumer_stdout_test du_high_scheduler_cell_metrics_consumer_stdout_test)
target_include_directories(du_high_scheduler_cell_metrics_consumer_stdout_test PRIVATE ${CMAKE_SOURCE_DIR})
