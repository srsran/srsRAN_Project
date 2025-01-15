#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(sched_ue_context_test ue_drx_controller_test.cpp)
target_link_libraries(sched_ue_context_test
        sched_ue_context
        srsran_support
        srslog
        gtest
        gtest_main)
add_test(sched_ue_context_test sched_ue_context_test)