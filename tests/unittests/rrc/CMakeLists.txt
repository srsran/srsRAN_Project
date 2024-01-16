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

set_directory_properties(PROPERTIES LABELS "rrc")

add_library(rrc_ue_test_helpers rrc_ue_test_messages.cpp)
target_include_directories(rrc_ue_test_helpers PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(rrc_ue_test_helpers srsran_support srslog)

set(SOURCES
        rrc_ue_setup_proc_test.cpp
        rrc_ue_dl_info_transfer_proc_test.cpp
        rrc_ue_smc_proc_test.cpp
        rrc_ue_reconfig_proc_test.cpp
        rrc_ue_capability_transfer_proc_test.cpp
        rrc_ue_reest_proc_test.cpp
        rrc_ue_meas_report_test.cpp)

add_executable(rrc_ue_test ${SOURCES})
target_include_directories(rrc_ue_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(rrc_ue_test rrc_ue_test_helpers srsran_e1ap srsran_rrc srsran_cu_cp_cell_meas_manager srsran_support srslog gtest gtest_main)
gtest_discover_tests(rrc_ue_test PROPERTIES "LABELS;rrc_ue")
