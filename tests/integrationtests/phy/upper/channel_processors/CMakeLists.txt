#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "phy")

add_executable(pxsch_bler_test
        pxsch_bler_test.cpp
        pxsch_bler_test_channel_emulator.cpp
        pxsch_bler_test_factories.cpp)
target_link_libraries(pxsch_bler_test
        srsran_ran
        srsran_upper_phy
        srsran_upper_phy_support
        srsran_channel_precoder
        srslog
        srsvec
        gtest
        gtest_main
)
gtest_discover_tests(pxsch_bler_test PROPERTIES LABELS "tsan" PROPERTIES LABELS "NO_MEMCHECK")

add_executable(pxsch_chain_test pxsch_chain_test.cpp)
target_link_libraries(pxsch_chain_test
        srsran_ran
        srsran_channel_processors
        srslog
        srsvec
        gtest
        gtest_main
)
gtest_discover_tests(pxsch_chain_test PROPERTIES LABELS "tsan" PROPERTIES LABELS "NO_MEMCHECK")
