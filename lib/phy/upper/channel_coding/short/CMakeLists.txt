#
# Copyright 2013-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(srsran_short_block STATIC short_block_encoder_impl.cpp short_block_detector_impl.cpp)
target_link_libraries(srsran_short_block srsran_support log_likelihood_ratio srsvec)