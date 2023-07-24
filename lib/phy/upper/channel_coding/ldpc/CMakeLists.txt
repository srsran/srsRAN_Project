#
# Copyright 2021-2023 Software Radio Systems Limited
#
# This file is part of srsRAN
#
# srsRAN is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsRAN is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#

set(ldpc_sources
        ldpc_graph_impl.cpp
        ldpc_luts_impl.cpp
        ldpc_encoder_impl.cpp
        ldpc_encoder_generic.cpp
        ldpc_decoder_impl.cpp
        ldpc_decoder_generic.cpp
        ldpc_rate_matcher_impl.cpp
        ldpc_rate_dematcher_impl.cpp
        ldpc_segmenter_impl.cpp
        )

if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
    list(APPEND ldpc_sources
            ldpc_encoder_avx2.cpp
            ldpc_decoder_avx2.cpp
            ldpc_decoder_avx512.cpp
            ldpc_rate_dematcher_avx2_impl.cpp
            ldpc_rate_dematcher_avx512_impl.cpp)
    set_source_files_properties(ldpc_rate_dematcher_avx2_impl.cpp PROPERTIES COMPILE_OPTIONS "-mavx2;")
    set_source_files_properties(ldpc_rate_dematcher_avx512_impl.cpp PROPERTIES COMPILE_OPTIONS "-mavx512f;-mavx512bw;")
    set_source_files_properties(ldpc_encoder_avx2.cpp PROPERTIES COMPILE_OPTIONS "-mavx2;")
    set_source_files_properties(ldpc_decoder_avx2.cpp PROPERTIES COMPILE_OPTIONS "-mavx2;")
    set_source_files_properties(ldpc_decoder_avx512.cpp PROPERTIES COMPILE_OPTIONS "-mavx512f;-mavx512bw;")
endif (${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")

if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64")
    list(APPEND ldpc_sources ldpc_encoder_neon.cpp)
    list(APPEND ldpc_sources ldpc_decoder_neon.cpp)
    list(APPEND ldpc_sources ldpc_rate_dematcher_neon_impl.cpp)
endif (${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64")

add_library(srsran_ldpc STATIC ${ldpc_sources})

target_link_libraries(srsran_ldpc log_likelihood_ratio)
