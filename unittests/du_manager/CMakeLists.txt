#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(du_test_suite du_test_suite.cpp)
target_link_libraries(du_test_suite srsgnb_ran gtest)

add_executable(ue_manager_test ue_manager_test.cpp)
target_link_libraries(ue_manager_test srsgnb_du_manager srsgnb_support srslog f1ap_asn1)
target_include_directories(ue_manager_test PRIVATE ${CMAKE_SOURCE_DIR})
add_test(ue_manager_test ue_manager_test)

add_executable(test_du_config_generator test_du_config_generator.cpp)
target_link_libraries(test_du_config_generator srsgnb_du_manager srsgnb_support du_test_suite gtest gtest_main)
add_test(test_du_config_generator test_du_config_generator)