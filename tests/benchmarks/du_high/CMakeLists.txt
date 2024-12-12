#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "du_high|tsan")

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
