#
# Copyright 2021-2024 Software Radio Systems Limited
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

set_directory_properties(PROPERTIES LABELS "phy;tsan;NO_MEMCHECK")

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
)
add_test(pxsch_bler_test pxsch_bler_test -R 10)

add_executable(pxsch_chain_test pxsch_chain_test.cpp)
target_link_libraries(pxsch_chain_test
        srsran_ran
        srsran_channel_processors
        srslog
        srsvec
        gtest
        gtest_main
)
gtest_discover_tests(pxsch_chain_test)
