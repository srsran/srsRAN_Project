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

set_directory_properties(PROPERTIES LABELS "sched|rtsan")

include_directories(../../..)

add_executable(scheduler_no_ues_benchmark scheduler_no_ues_benchmark.cpp)
target_link_libraries(scheduler_no_ues_benchmark srsran_sched scheduler_test_utils srslog sched_config)
add_test(scheduler_no_ues_benchmark scheduler_no_ues_benchmark)

add_executable(scheduler_multi_ue_benchmark scheduler_multi_ue_benchmark.cpp)
target_link_libraries(scheduler_multi_ue_benchmark srsran_sched sched_test_doubles srslog sched_config)
add_test(scheduler_multi_ue_benchmark scheduler_multi_ue_benchmark)
