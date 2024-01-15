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

include_directories(../../..)

add_library(f1ap_du_test_helpers f1ap_du_test_helpers.cpp)
target_link_libraries(f1ap_du_test_helpers f1ap_test_helpers srsran_f1ap_du srsran_support du_manager_converters sched_config)

set(SOURCES f1ap_du_setup_procedure_test.cpp
            f1ap_du_dl_rrc_message_transfer_test.cpp
            f1ap_du_ue_config_test.cpp
            f1ap_du_ue_context_setup_procedure_test.cpp
            f1ap_du_ue_context_release_test.cpp
            f1ap_du_ue_context_modification_test.cpp
        f1ap_du_ul_rrc_message_transfer_test.cpp)

add_executable(f1ap_du_test ${SOURCES})
target_include_directories(f1ap_du_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(f1ap_du_test
        srsran_f1ap_du
        srsran_support
        du_manager_converters
        srsran_ran
        sched_config
        f1ap_du_test_helpers
        gtest
        gtest_main)
gtest_discover_tests(f1ap_du_test PROPERTIES "LABELS;f1ap_du")
