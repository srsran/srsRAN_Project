#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(rlc_stress_test_mac rlc_stress_test_mac.cpp)
add_library(rlc_stress_test_traffic rlc_stress_test_traffic.cpp)
add_executable(rlc_stress_test rlc_stress_test.cpp)

target_link_libraries(rlc_stress_test rlc_stress_test_traffic rlc_stress_test_mac srsgnb_cu_cp srsgnb_rlc srsgnb_support srslog )
target_include_directories(rlc_stress_test PRIVATE ${CMAKE_SOURCE_DIR})

#
# Test different RLC modes (PDCP using EIA2/EEA2 for validation)
#

# UM6 (PDCP: EIA2/EEA2)
add_test(rlc_um6_eia2_eea2_stress_test rlc_stress_test --mode=UM6 --pdcp_integrity_algo=2 --pdcp_ciphering_algo=2)
set_tests_properties(rlc_um6_eia2_eea2_stress_test PROPERTIES TIMEOUT 240)
set_tests_properties(rlc_um6_eia2_eea2_stress_test PROPERTIES LABELS "tsan")

# UM12 (PDCP: EIA2/EEA2)
add_test(rlc_um12_eia2_eea2_stress_test rlc_stress_test --mode=UM12 --pdcp_integrity_algo=2 --pdcp_ciphering_algo=2)
set_tests_properties(rlc_um12_eia2_eea2_stress_test PROPERTIES TIMEOUT 240)
set_tests_properties(rlc_um12_eia2_eea2_stress_test PROPERTIES LABELS "tsan")

# AM12 (PDCP: EIA2/EEA2)
add_test(rlc_am12_eia2_eea2_stress_test rlc_stress_test --mode=AM12 --pdcp_integrity_algo=2 --pdcp_ciphering_algo=2)
set_tests_properties(rlc_am12_eia2_eea2_stress_test PROPERTIES TIMEOUT 240)
set_tests_properties(rlc_am12_eia2_eea2_stress_test PROPERTIES LABELS "tsan")

# AM18 (PDCP: EIA2/EEA2)
add_test(rlc_am18_eia2_eea2_stress_test rlc_stress_test --mode=AM18 --pdcp_integrity_algo=2 --pdcp_ciphering_algo=2)
set_tests_properties(rlc_am18_eia2_eea2_stress_test PROPERTIES TIMEOUT 240)
set_tests_properties(rlc_am18_eia2_eea2_stress_test PROPERTIES LABELS "tsan")

#
# Test one RLC mode with different cipher/integrity algorithms in the PDCP
#

# UM12 (PDCP: EIA1/EEA1)
# Note: Until EIA1/EEA1 is optimized, only test 50 TTIs instead of 500 (default)
add_test(rlc_um12_eia1_eea1_stress_test rlc_stress_test --mode=UM12 --pdcp_integrity_algo=1 --pdcp_ciphering_algo=1 --nof_ttis=50)
set_tests_properties(rlc_um12_eia1_eea1_stress_test PROPERTIES TIMEOUT 240)
set_tests_properties(rlc_um12_eia1_eea1_stress_test PROPERTIES LABELS "tsan")

# UM12 (PDCP: EIA3/EEA3)
add_test(rlc_um12_eia3_eea3_stress_test rlc_stress_test --mode=UM12 --pdcp_integrity_algo=3 --pdcp_ciphering_algo=3)
set_tests_properties(rlc_um12_eia3_eea3_stress_test PROPERTIES TIMEOUT 240)
set_tests_properties(rlc_um12_eia3_eea3_stress_test PROPERTIES LABELS "tsan")

# UM12 (PDCP: EIA2/EEA0)
add_test(rlc_um12_eia2_eea0_stress_test rlc_stress_test --mode=UM12 --pdcp_integrity_algo=2 --pdcp_ciphering_algo=0)
set_tests_properties(rlc_um12_eia2_eea0_stress_test PROPERTIES TIMEOUT 240)
set_tests_properties(rlc_um12_eia2_eea0_stress_test PROPERTIES LABELS "tsan")
