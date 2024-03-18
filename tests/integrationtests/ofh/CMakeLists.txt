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

set_directory_properties(PROPERTIES LABELS "ofh")

add_executable(ofh_integration_test ofh_integration_test.cpp)
target_link_libraries(ofh_integration_test srslog srsran_ru_ofh srsran_phy_support srsran_support)
add_test(ofh_integration_test ofh_integration_test)
set_tests_properties(ofh_integration_test PROPERTIES LABELS "tsan;NO_MEMCHECK")

if (DPDK_FOUND)
    add_executable(ru_emulator ru_emulator_cli11_schema.cpp ru_emulator.cpp)
    target_compile_options(ru_emulator PRIVATE ${DPDK_CFLAGS})

    target_link_libraries(ru_emulator srslog srsran_ofh_dpdk_ethernet
            srsran_phy_support srsran_support hal_dpdk ${DPDK_LIBRARIES})

    target_include_directories(ru_emulator PRIVATE ${CMAKE_SOURCE_DIR})
    include_directories(${CMAKE_SOURCE_DIR}/external)
endif (DPDK_FOUND)
