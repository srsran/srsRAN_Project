#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set_directory_properties(PROPERTIES LABELS "phy")

add_executable(pseudo_random_test pseudo_random_generator_test.cpp)
target_link_libraries(pseudo_random_test srsgnb_sequence_generators srsvec srslog)
add_test(pseudo_random_test pseudo_random_test)

add_executable(low_papr_sequence_generator_test low_papr_sequence_generator_test.cpp)
target_link_libraries(low_papr_sequence_generator_test srsgnb_sequence_generators srsvec srslog srsgnb_support)
add_test(low_papr_sequence_generator_test low_papr_sequence_generator_test)
