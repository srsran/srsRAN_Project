#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(ng_setup_procedure_test ng_setup_procedure_test.cpp)
target_link_libraries(ng_setup_procedure_test
                      srsgnb_ngap
                      srsgnb_support
                      srslog
                      ngap_asn1
                      srsgnb_ran
                      gtest
                      gtest_main)
target_include_directories(ng_setup_procedure_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(ng_setup_procedure_test)

add_executable(ngc_asn1_packer_test ngc_asn1_packer_test.cpp)
target_link_libraries(ngc_asn1_packer_test
                      srsgnb_ngap
                      srsgnb_support
                      srslog
                      ngap_asn1
                      srsgnb_ran
                      gtest
                      gtest_main)
target_include_directories(ngc_asn1_packer_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(ngc_asn1_packer_test)

add_executable(ng_nas_messages_procedure_test ng_nas_messages_procedure_test.cpp)
target_link_libraries(ng_nas_messages_procedure_test
                      srsgnb_ngap
                      srsgnb_support
                      srslog
                      ngap_asn1
                      srsgnb_ran
                      gtest
                      gtest_main)
target_include_directories(ng_nas_messages_procedure_test PRIVATE ${CMAKE_SOURCE_DIR})
gtest_discover_tests(ng_nas_messages_procedure_test)