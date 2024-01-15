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

add_executable(common_scheduler_test
        ssb_scheduler_test.cpp
        sib1_scheduler_test.cpp
        prach_scheduler_test.cpp
        ra_scheduler_test.cpp
        paging_scheduler_test.cpp)
target_link_libraries(common_scheduler_test
        srsran_sched
        srslog
        sched_config
        mac_configuration_helpers
        scheduler_test_suite
        gtest
        gtest_main)
gtest_discover_tests(common_scheduler_test)
