#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "sched|rtsan")

include_directories(../../..)

add_executable(scheduler_no_ues_benchmark scheduler_no_ues_benchmark.cpp)
target_link_libraries(scheduler_no_ues_benchmark srsran_sched scheduler_test_utils srslog sched_config)
add_test(scheduler_no_ues_benchmark scheduler_no_ues_benchmark)

add_executable(scheduler_multi_ue_benchmark scheduler_multi_ue_benchmark.cpp)
target_link_libraries(scheduler_multi_ue_benchmark srsran_sched sched_test_doubles srslog sched_config)
add_test(scheduler_multi_ue_benchmark scheduler_multi_ue_benchmark)
