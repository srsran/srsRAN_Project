#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(ue_manager_test ue_manager_test.cpp)
target_link_libraries(ue_manager_test srsgnb_du_manager srsgnb_support srslog f1ap_asn1)
target_include_directories(ue_manager_test PRIVATE ${CMAKE_SOURCE_DIR})
add_test(ue_manager_test ue_manager_test)
