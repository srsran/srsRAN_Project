#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_executable(ldpc_encoder_benchmark ldpc_encoder_benchmark.cpp)
target_link_libraries(ldpc_encoder_benchmark srsgnb_channel_coding srslog)
add_test(ldpc_encoder_benchmark ldpc_encoder_benchmark -s -R 1 -T generic)

add_executable(ldpc_decoder_benchmark ldpc_decoder_benchmark.cpp)
target_link_libraries(ldpc_decoder_benchmark srsgnb_channel_coding srslog)
add_test(ldpc_decoder_benchmark ldpc_decoder_benchmark -s -R 1 -I 2 -T generic)
