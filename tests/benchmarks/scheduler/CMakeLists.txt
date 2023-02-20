#
#Copyright 2013 - 2022 Software Radio Systems Limited
#
#By using this file, you agree to the terms and conditions set
#forth in the LICENSE file which can be found at the top level of
#the distribution.
#

set_directory_properties(PROPERTIES LABELS "sched")

include_directories(../../..)

add_executable(scheduler_no_ues_benchmark scheduler_no_ues_benchmark.cpp)
target_link_libraries(scheduler_no_ues_benchmark srsran_sched srslog sched_config)
