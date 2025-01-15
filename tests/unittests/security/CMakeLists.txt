#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "security")

add_executable(security_test security_test.cpp)
target_link_libraries(security_test srsran_security srsran_support srslog gtest gtest_main)
gtest_discover_tests(security_test)

add_executable(integrity_engine_test integrity_engine_test.cpp)
target_include_directories(integrity_engine_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(integrity_engine_test srsran_security srsran_support srslog gtest gtest_main)
gtest_discover_tests(integrity_engine_test)

add_executable(ciphering_engine_test ciphering_engine_test.cpp)
target_include_directories(ciphering_engine_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(ciphering_engine_test srsran_security srsran_support srslog gtest gtest_main)
gtest_discover_tests(ciphering_engine_test)

add_executable(k_gnb_star_gen_helper k_gnb_star_gen_helper.cpp)
target_include_directories(k_gnb_star_gen_helper PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(k_gnb_star_gen_helper srsran_security srsran_support srslog)
