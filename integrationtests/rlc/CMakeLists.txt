#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(rlc_stress_test_mac rlc_stress_test_mac.cpp)
add_library(rlc_stress_test_pdcp rlc_stress_test_pdcp.cpp)
add_executable(rlc_stress_test rlc_stress_test.cpp)

target_link_libraries(rlc_stress_test rlc_stress_test_pdcp rlc_stress_test_mac srsgnb_cu_cp srsgnb_rlc srsgnb_support srslog )
target_include_directories(rlc_stress_test PRIVATE ${CMAKE_SOURCE_DIR})


#add_nr_test(rlc_um6_stress_test rlc_stress_test --mode=UM6 --loglevel 1)
#add_nr_test(rlc_um12_stress_test rlc_stress_test --mode=UM12 --loglevel 1)
#add_nr_test(rlc_am12_stress_test rlc_stress_test --mode=AM12 --loglevel 1)
#add_nr_test(rlc_am12_stress_test rlc_stress_test --mode=AM18 --loglevel 1)
