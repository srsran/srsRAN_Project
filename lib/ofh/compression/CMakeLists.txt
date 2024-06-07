#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        compressed_prb_packer.cpp
        compressed_prb_unpacker.cpp
        compression_factory.cpp
        iq_compression_none_impl.cpp
        iq_compression_bfp_impl.cpp
        iq_compression_death_impl.cpp
        iq_compressor_selector.cpp
        iq_decompressor_selector.cpp)

if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
    list(APPEND SOURCES
            iq_compression_bfp_avx2.cpp
            iq_compression_bfp_avx512.cpp
            iq_compression_none_avx512.cpp
            iq_compression_none_avx2.cpp)
    set_source_files_properties(iq_compression_bfp_avx2.cpp PROPERTIES COMPILE_OPTIONS "-mavx2;")
    set_source_files_properties(iq_compression_none_avx2.cpp PROPERTIES COMPILE_OPTIONS "-mavx2;")
    set_source_files_properties(iq_compression_bfp_avx512.cpp PROPERTIES
            COMPILE_OPTIONS "-mavx512f;-mavx512bw;-mavx512vl;-mavx512cd;-mavx512dq;")
    set_source_files_properties(iq_compression_none_avx512.cpp PROPERTIES
            COMPILE_OPTIONS "-mavx512f;-mavx512bw;-mavx512vl;-mavx512dq;")
endif (${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")

if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64")
    list(APPEND SOURCES iq_compression_bfp_neon.cpp)
    list(APPEND SOURCES iq_compression_none_neon.cpp)
endif (${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64")

add_library(srsran_ofh_compression STATIC ${SOURCES})
target_link_libraries(srsran_ofh_compression srsvec)
