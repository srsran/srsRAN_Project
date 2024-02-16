#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#
set_directory_properties(PROPERTIES LABELS "phy")

add_executable(pxsch_chain_test pxsch_chain_test.cpp)
target_link_libraries(pxsch_chain_test
        srsran_ran
        srsran_channel_processors
        srslog
        srsvec
        gtest
        gtest_main
)
add_test(pxsch_chain_test pxsch_chain_test)
set_tests_properties(pxsch_chain_test PROPERTIES LABELS "tsan;NO_MEMCHECK")
