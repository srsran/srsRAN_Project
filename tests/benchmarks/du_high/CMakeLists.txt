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

set_directory_properties(PROPERTIES LABELS "du_high|tsan|rtsan")

include_directories(../../..)

add_executable(du_high_benchmark du_high_benchmark.cpp)
target_link_libraries(du_high_benchmark
  srsran_du_high
  f1ap_du_test_helpers
  pdcp_test_doubles
  sched_test_doubles
  mac_test_doubles
  du_test_doubles
  srsran_pcap
  gtest
)
add_test(du_high_benchmark du_high_benchmark)
