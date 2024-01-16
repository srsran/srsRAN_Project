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

set_directory_properties(PROPERTIES LABELS "rlc")

add_library(rlc_stress_test_mac rlc_stress_test_mac.cpp)
target_include_directories(rlc_stress_test_mac PRIVATE ${CMAKE_SOURCE_DIR})
add_library(rlc_stress_test_traffic rlc_stress_test_traffic.cpp)
target_include_directories(rlc_stress_test_traffic PRIVATE ${CMAKE_SOURCE_DIR})
add_executable(rlc_stress_test rlc_stress_test.cpp)
target_include_directories(rlc_stress_test PRIVATE ${CMAKE_SOURCE_DIR})

target_link_libraries(rlc_stress_test rlc_stress_test_traffic rlc_stress_test_mac srsran_cu_cp srsran_rlc srsran_support srslog )
target_include_directories(rlc_stress_test PRIVATE ${CMAKE_SOURCE_DIR})

#
# Test different RLC modes (PDCP using EIA2/EEA2 for validation)
#

# UM6 (PDCP: EIA2/EEA2)
add_test(rlc_um6_eia2_eea2_stress_test rlc_stress_test --mode=UM6 --pdcp_integrity_algo=2 --pdcp_ciphering_algo=2)
set_tests_properties(rlc_um6_eia2_eea2_stress_test PROPERTIES LABELS "tsan")

# UM12 (PDCP: EIA2/EEA2)
add_test(rlc_um12_eia2_eea2_stress_test rlc_stress_test --mode=UM12 --pdcp_integrity_algo=2 --pdcp_ciphering_algo=2)
set_tests_properties(rlc_um12_eia2_eea2_stress_test PROPERTIES LABELS "tsan")

# AM12 (PDCP: EIA2/EEA2)
add_test(rlc_am12_eia2_eea2_stress_test rlc_stress_test --mode=AM12 --pdcp_integrity_algo=2 --pdcp_ciphering_algo=2)
set_tests_properties(rlc_am12_eia2_eea2_stress_test PROPERTIES LABELS "tsan")

# AM18 (PDCP: EIA2/EEA2)
add_test(rlc_am18_eia2_eea2_stress_test rlc_stress_test --mode=AM18 --pdcp_integrity_algo=2 --pdcp_ciphering_algo=2)
set_tests_properties(rlc_am18_eia2_eea2_stress_test PROPERTIES LABELS "tsan")

#
# Test one RLC mode with different cipher/integrity algorithms in the PDCP
#

# UM12 (PDCP: EIA1/EEA1)
# Note: Until EIA1/EEA1 is optimized, only test 50 TTIs instead of 500 (default)
add_test(rlc_um12_eia1_eea1_stress_test rlc_stress_test --mode=UM12 --pdcp_integrity_algo=1 --pdcp_ciphering_algo=1 --nof_ttis=50)
set_tests_properties(rlc_um12_eia1_eea1_stress_test PROPERTIES LABELS "tsan")

# UM12 (PDCP: EIA3/EEA3)
add_test(rlc_um12_eia3_eea3_stress_test rlc_stress_test --mode=UM12 --pdcp_integrity_algo=3 --pdcp_ciphering_algo=3)
set_tests_properties(rlc_um12_eia3_eea3_stress_test PROPERTIES LABELS "tsan")

# UM12 (PDCP: EIA2/EEA0)
add_test(rlc_um12_eia2_eea0_stress_test rlc_stress_test --mode=UM12 --pdcp_integrity_algo=2 --pdcp_ciphering_algo=0)
set_tests_properties(rlc_um12_eia2_eea0_stress_test PROPERTIES LABELS "tsan")
