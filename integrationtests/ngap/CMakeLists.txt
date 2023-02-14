#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(ngap_integration_test ngap_integration_test.cpp)
target_link_libraries(ngap_integration_test  srslog srsgnb_io_broker srsgnb_ngap ngap_asn1 srsgnb_gateway srsgnb_support gtest gtest_main)
target_include_directories(ngap_integration_test PRIVATE ${CMAKE_SOURCE_DIR})
#gtest_discover_tests(ngap_integration_test)
