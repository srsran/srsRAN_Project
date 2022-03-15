#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

include_directories(${PROJECT_SOURCE_DIR}/include/srsgnb/phy/upper/channel_coding)
add_library(ldpc STATIC
        ldpc_graph_impl.cpp
        ldpc_luts_impl.cpp
        ldpc_encoder_impl.cpp
        ldpc_decoder_impl.cpp
        ldpc_rate_matcher_impl.cpp
        ldpc_rate_dematcher_impl.cpp
        ldpc_segmenter_impl.cpp
)
