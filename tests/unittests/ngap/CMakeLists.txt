#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(ngap_test_helpers ngap_test_helpers.cpp ngap_test_messages.cpp)
target_include_directories(ngap_test_helpers PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(ngap_test_helpers srsgnb_support ngap_asn1 srslog)

set(SOURCES
        ngap_asn1_packer_test.cpp
        ng_setup_procedure_test.cpp
        ngap_nas_message_test.cpp
        ngap_ue_context_management_procedure_test.cpp
        ngap_pdu_session_resource_setup_procedure_test.cpp)

add_executable(ngap_test ${SOURCES})
target_include_directories(ngap_test PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(ngap_test ngap_test_helpers srsgnb_ngap srsgnb_support ngap_asn1 srsgnb_ran srslog gtest gtest_main)
gtest_discover_tests(ngap_test PROPERTIES "LABELS;ngap")
