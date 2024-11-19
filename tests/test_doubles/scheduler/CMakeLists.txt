#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(sched_test_doubles pucch_res_test_builder_helper.cpp scheduler_result_test.cpp
        cell_config_builder_profiles.cpp
        scheduler_config_helper.cpp)
set_target_properties(sched_test_doubles PROPERTIES UNITY_BUILD ON)
target_link_libraries(sched_test_doubles srsran_sched srslog)
