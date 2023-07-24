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
    list(APPEND SOURCES packing_utils_avx2.cpp
            iq_compression_bfp_avx2.cpp
            packing_utils_avx512.cpp
            iq_compression_bfp_avx512.cpp)
    set_source_files_properties(packing_utils_avx2.cpp PROPERTIES COMPILE_OPTIONS "-mavx2;")
    set_source_files_properties(iq_compression_bfp_avx2.cpp PROPERTIES COMPILE_OPTIONS "-mavx2;")
    set_source_files_properties(packing_utils_avx512.cpp PROPERTIES
            COMPILE_OPTIONS "-mavx512f;-mavx512bw;-mavx512vl;-mavx512dq;")
    set_source_files_properties(iq_compression_bfp_avx512.cpp PROPERTIES
            COMPILE_OPTIONS "-mavx512f;-mavx512bw;-mavx512vl;-mavx512cd;")
endif (${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")

if (HAVE_NEON)
    list(APPEND SOURCES packing_utils_neon.cpp)
    list(APPEND SOURCES iq_compression_bfp_neon.cpp)
endif (HAVE_NEON)

add_library(srsran_ofh_compression STATIC ${SOURCES})
target_link_libraries(srsran_ofh_compression srsvec)
