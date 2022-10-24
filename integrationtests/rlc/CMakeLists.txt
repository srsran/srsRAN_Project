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

# UM6
add_test(rlc_um6_stress_test rlc_stress_test --mode=UM6)
set_tests_properties(rlc_um6_stress_test PROPERTIES TIMEOUT 240) 
set_tests_properties(rlc_um6_stress_test PROPERTIES LABELS "tsan")

# UM12
add_test(rlc_um12_stress_test rlc_stress_test --mode=UM12)
set_tests_properties(rlc_um12_stress_test PROPERTIES TIMEOUT 240) 
set_tests_properties(rlc_um12_stress_test PROPERTIES LABELS "tsan")

# AM12
add_test(rlc_am12_stress_test rlc_stress_test --mode=AM12)
set_tests_properties(rlc_am12_stress_test PROPERTIES TIMEOUT 240) 
set_tests_properties(rlc_am12_stress_test PROPERTIES LABELS "tsan")

# AM18
add_test(rlc_am18_stress_test rlc_stress_test --mode=AM18)
set_tests_properties(rlc_am18_stress_test PROPERTIES TIMEOUT 240) 
set_tests_properties(rlc_am18_stress_test PROPERTIES LABELS "tsan")
