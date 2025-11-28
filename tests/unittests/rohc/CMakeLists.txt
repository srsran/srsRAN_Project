#
# Copyright 2021-2025 Software Radio Systems Limited
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

set_directory_properties(PROPERTIES LABELS "rohc")

add_executable(rohc_config_test rohc_config_test.cpp)
target_link_libraries(rohc_config_test srsran_rohc srsran_support srslog gtest gtest_main)
gtest_discover_tests(rohc_config_test)

add_executable(rohc_factory_test rohc_factory_test.cpp)
target_link_libraries(rohc_factory_test srsran_rohc srsran_support srslog gtest gtest_main)
gtest_discover_tests(rohc_factory_test)

if(ROHC_FOUND)
  add_executable(rohc_lib_engine_test rohc_lib_engine_test.cpp)
  target_include_directories(rohc_lib_engine_test PRIVATE ${CMAKE_SOURCE_DIR})
  target_link_libraries(rohc_lib_engine_test srsran_rohc srsran_support srslog gtest gtest_main)
  gtest_discover_tests(rohc_lib_engine_test)
endif()
