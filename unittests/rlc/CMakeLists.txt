#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "rlc")

add_executable(rlc_tm_test rlc_tm_test.cpp)
target_link_libraries(rlc_tm_test srsgnb_rlc srsgnb_support srslog)
add_test(rlc_tm_test rlc_tm_test)

add_executable(rlc_um_pdu_test rlc_um_pdu_test.cpp)
target_link_libraries(rlc_um_pdu_test srsgnb_support srslog)
add_test(rlc_um_pdu_test rlc_um_pdu_test)

add_executable(rlc_am_pdu_test rlc_am_pdu_test.cpp)
target_link_libraries(rlc_am_pdu_test srsgnb_rlc srsgnb_support srslog)
add_test(rlc_am_pdu_test rlc_am_pdu_test)

add_executable(rlc_byte_buffer_test rlc_byte_buffer_test.cpp)
target_link_libraries(rlc_byte_buffer_test srslog)
add_test(rlc_byte_buffer_test rlc_byte_buffer_test)

add_executable(rlc_sdu_queue_test rlc_sdu_queue_test.cpp)
target_link_libraries(rlc_sdu_queue_test srsgnb_support srslog)
add_test(rlc_sdu_queue_test rlc_sdu_queue_test)
