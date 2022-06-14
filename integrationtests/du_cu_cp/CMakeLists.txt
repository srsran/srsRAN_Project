#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(cu_du_test cu_du_test.cpp)
target_link_libraries(cu_du_test srsgnb_cu_cp srsgnb_du_high srsgnb_du_config srsgnb_f1 srsgnb_support srslog f1ap_asn1)
target_include_directories(cu_du_test PRIVATE ${CMAKE_SOURCE_DIR})
add_test(cu_du_test cu_du_test)