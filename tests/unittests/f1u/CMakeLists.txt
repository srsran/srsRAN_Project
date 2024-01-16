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

add_subdirectory(cu_up)
add_subdirectory(du)

add_executable(f1u_local_connector_test common/f1u_connector_test.cpp)
target_link_libraries(f1u_local_connector_test
srsgnb_app_f1u_connector
srsran_f1u_du
srsran_f1u_cu_up
srsran_support
srslog
gtest
gtest_main)

target_include_directories(f1u_local_connector_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(f1u_local_connector_test)
