#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

include_directories(${PROJECT_SOURCE_DIR}/include/srsgnb/phy/upper/channel_coding/ldpc)

set(ldpc_sources
  ldpc_graph_impl.cpp
  ldpc_luts_impl.cpp
  ldpc_encoder_impl.cpp
  ldpc_decoder_impl.cpp
  ldpc_rate_matcher_impl.cpp
  ldpc_rate_dematcher_impl.cpp
  ldpc_segmenter_impl.cpp
)

if(ENABLE_AVX2)
  list(APPEND ldpc_sources
    ldpc_decoder_avx2.cpp
    ldpc_encoder_avx2.cpp
    ldpc_rate_dematcher_avx2_impl.cpp)
  set_source_files_properties(ldpc_rate_dematcher_avx2_impl.cpp PROPERTIES COMPILE_OPTIONS "-mavx2;")
endif(ENABLE_AVX2)

if(ENABLE_AVX512)
  list(APPEND ldpc_sources ldpc_rate_dematcher_avx512_impl.cpp)
  set_source_files_properties(ldpc_rate_dematcher_avx512_impl.cpp PROPERTIES COMPILE_OPTIONS "-mavx512f;-mavx512bw;")
endif(ENABLE_AVX512)

if(HAVE_AVX512)
  list(APPEND ldpc_sources ldpc_decoder_avx512.cpp)
endif(HAVE_AVX512)

add_library(srsgnb_ldpc STATIC ${ldpc_sources})

target_link_libraries(srsgnb_ldpc log_likelihood_ratio)

install(TARGETS srsgnb_ldpc EXPORT srsgnb_export)
