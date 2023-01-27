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

if (HAVE_AVX2)
    list(APPEND ldpc_sources
            ldpc_decoder_avx2.cpp
            ldpc_encoder_avx2.cpp)
endif (HAVE_AVX2)

if (HAVE_AVX512)
    list(APPEND ldpc_sources ldpc_decoder_avx512.cpp)
endif (HAVE_AVX512)

if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
    list(APPEND ldpc_sources
            ldpc_rate_dematcher_avx2_impl.cpp
            ldpc_rate_dematcher_avx512_impl.cpp)
    set_source_files_properties(ldpc_rate_dematcher_avx2_impl.cpp PROPERTIES COMPILE_OPTIONS "-mavx2;")
    set_source_files_properties(ldpc_rate_dematcher_avx512_impl.cpp PROPERTIES COMPILE_OPTIONS "-mavx512f;-mavx512bw;")
endif (${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")

if(HAVE_NEON)
  list(APPEND ldpc_sources ldpc_encoder_neon.cpp)
  list(APPEND ldpc_sources ldpc_decoder_neon.cpp)
  list(APPEND ldpc_sources ldpc_rate_dematcher_neon_impl.cpp)
endif(HAVE_NEON)

add_library(srsgnb_ldpc STATIC ${ldpc_sources})

target_link_libraries(srsgnb_ldpc log_likelihood_ratio)

install(TARGETS srsgnb_ldpc EXPORT srsgnb_export)
