#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(cu_up)
add_subdirectory(du)

add_executable(f1u_local_connector_test common/f1u_connector_test.cpp)
target_link_libraries(f1u_local_connector_test
                      srsgnbapp_f1u_connector
                      srsgnb_f1u_du
                      srsgnb_f1u_cu_up
                      srsgnb_support
                      srslog
                      gtest
                      gtest_main)

target_include_directories(f1u_local_connector_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(f1u_local_connector_test)
