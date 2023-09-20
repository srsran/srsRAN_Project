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

include(CheckCSourceRuns)

option(ENABLE_SSE    "Enable compile-time SSE4.1 support." ON)
option(ENABLE_AVX    "Enable compile-time AVX support."    ON)
option(ENABLE_AVX2   "Enable compile-time AVX2 support."   ON)
option(ENABLE_FMA    "Enable compile-time FMA support."    ON)
option(ENABLE_AVX512 "Enable compile-time AVX512 support." ON)

# Unset cached variables if corresponding options are disabled
if(NOT ENABLE_SSE)
    unset(HAVE_SSE CACHE)
endif()
if(NOT ENABLE_AVX)
    unset(HAVE_AVX CACHE)
endif()
if(NOT ENABLE_AVX2)
    unset(HAVE_AVX2 CACHE)
endif()
if(NOT ENABLE_FMA)
    unset(HAVE_FMA CACHE)
endif()
if(NOT ENABLE_AVX)
    unset(ENABLE_AVX512 CACHE)
endif()

if (ENABLE_SSE)
    #
    # Check compiler for SSE4_1 intrinsics
    #
    if (CMAKE_COMPILER_IS_GNUCC OR (CMAKE_C_COMPILER_ID MATCHES "Clang") OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
        set(CMAKE_REQUIRED_FLAGS "-msse4.1")
        check_c_source_runs("
        #include <emmintrin.h>
        #include <smmintrin.h>

        int main()
        {
        __m128i a = _mm_setzero_si128();
        __m128i b = _mm_minpos_epu16(a);
        return 0;
        }"
                HAVE_SSE)
    endif()

    if (HAVE_SSE)
        message(STATUS "SSE4.1 is enabled - target CPU must support it")
    endif()

    if (ENABLE_AVX)

        #
        # Check compiler for AVX intrinsics
        #
        if (CMAKE_COMPILER_IS_GNUCC OR (CMAKE_C_COMPILER_ID MATCHES "Clang") OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
            set(CMAKE_REQUIRED_FLAGS "-mavx")
            check_c_source_runs("
            #include <immintrin.h>
            int main()
            {
              __m256 a, b, c;
              const float src[8] = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f };
              float dst[8];
              a = _mm256_loadu_ps( src );
              b = _mm256_loadu_ps( src );
              c = _mm256_add_ps( a, b );
              _mm256_storeu_ps( dst, c );
              int i = 0;
              for( i = 0; i < 8; i++ ){
                if( ( src[i] + src[i] ) != dst[i] ){
                  return -1;
                }
              }
              return 0;
            }"
                    HAVE_AVX)
        endif()

        if (HAVE_AVX)
            message(STATUS "AVX is enabled - target CPU must support it")
        endif()
    endif()

    if (ENABLE_AVX2)

        #
        # Check compiler for AVX intrinsics
        #
        if (CMAKE_COMPILER_IS_GNUCC OR (CMAKE_C_COMPILER_ID MATCHES "Clang") OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
            set(CMAKE_REQUIRED_FLAGS "-mavx2")
            check_c_source_runs("
          #include <immintrin.h>
          int main()
          {
            __m256i a, b, c;
            const int src[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
            int dst[8];
            a =  _mm256_loadu_si256( (__m256i*)src );
            b =  _mm256_loadu_si256( (__m256i*)src );
            c = _mm256_add_epi32( a, b );
            _mm256_storeu_si256( (__m256i*)dst, c );
            int i = 0;
            for( i = 0; i < 8; i++ ){
              if( ( src[i] + src[i] ) != dst[i] ){
                return -1;
              }
            }
            return 0;
          }"
                    HAVE_AVX2)
        endif()

        if (HAVE_AVX2)
            message(STATUS "AVX2 is enabled - target CPU must support it")
        endif()
    endif()

    if (ENABLE_FMA)

        #
        # Check compiler for AVX intrinsics
        #
        if (CMAKE_COMPILER_IS_GNUCC OR (CMAKE_C_COMPILER_ID MATCHES "Clang") OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
            set(CMAKE_REQUIRED_FLAGS "-mfma")
            check_c_source_runs("
            #include <immintrin.h>
            int main()
            {
              __m256 a, b, c, r;
              const float src[8] = { 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f };
              float dst[8];
              a = _mm256_loadu_ps( src );
              b = _mm256_loadu_ps( src );
              c = _mm256_loadu_ps( src );
              r = _mm256_fmadd_ps( a, b, c );
              _mm256_storeu_ps( dst, r );
              int i = 0;
              for( i = 0; i < 8; i++ ){
                if( ( src[i] * src[i] + src[i] ) != dst[i] ){
                  return -1;
                }
              }
              return 0;
            }"
                    HAVE_FMA)
        endif()

        if (HAVE_FMA)
            message(STATUS "FMA is enabled - target CPU must support it")
        endif()
    endif()

    if (ENABLE_AVX512)

        #
        # Check compiler for AVX512 intrinsics
        #
        if (CMAKE_COMPILER_IS_GNUCC OR (CMAKE_C_COMPILER_ID MATCHES "Clang") OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
            set(CMAKE_REQUIRED_FLAGS "-mavx512f -mavx512cd -mavx512bw -mavx512dq -DLV_HAVE_AVX512")
            check_c_source_runs("
          #include <immintrin.h>
          int main()
          {
            __m512i a, b, c;
            const int src[16] = { 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8 , 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF};
            int dst[16];
            a =  _mm512_loadu_si512( (__m512i*)src );
            b =  _mm512_loadu_si512( (__m512i*)src );
            c = _mm512_add_epi32( a, b );
            _mm512_storeu_si512( (__m512i*)dst, c );
            int i = 0;
            for( i = 0; i < 16; i++ ){
              if( ( src[i] + src[i] ) != dst[i] ){
                return -1;
              }
            }
            return 0;
          }"
                    HAVE_AVX512)
        endif()

        if (HAVE_AVX512)
            message(STATUS "AVX512 is enabled - target CPU must support it")

            if (NOT HAVE_AVX)
                message(WARNING "AVX512 is enabled but AVX is not. This has not been tested. Enable AVX if you can.")
            endif()

            if (NOT HAVE_AVX2)
                message(WARNING "AVX512 is enabled but AVX2 is not. This has not been tested. Enable AVX2 if you can.")
            endif()

        endif()
    elseif (${GCC_ARCH} MATCHES "native")
        # When a GNU compiler flag -march=native is present and the CPU supports AVX512, the compiler enables
        # automatically all the supported AVX512 flags automatically.

        # Get the CPU AVX512 enabled flags.
        if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
            execute_process(COMMAND bash -c "${CMAKE_CXX_COMPILER} -march=native -Q --help=target | grep -E \"\\-m.*avx512.*\\[enabled]\" -o | cut -c 3- | grep -E \"^\\w*\" -o"
                    OUTPUT_VARIABLE NATIVE_AVX512_FLAGS)
            STRING(REGEX REPLACE "\n" ";" NATIVE_AVX512_FLAGS "${NATIVE_AVX512_FLAGS}")
        elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            execute_process(COMMAND bash -c "echo 'int main(){}' | ${CMAKE_CXX_COMPILER} -march=native -S -emit-llvm -x c - -o - | grep attributes. | grep -E \"\\+avx512\\w*,\" -o | cut -c 2-"
                    OUTPUT_VARIABLE NATIVE_AVX512_FLAGS)
            STRING(REGEX REPLACE ",\n" ";" NATIVE_AVX512_FLAGS "${NATIVE_AVX512_FLAGS}")
        else()
            message(WARNING "AVX512 detection is disabled but AVX512 flags are not disabled.")
        endif()

        # Disable all the AVX512 related flags.
        foreach (FLAG ${NATIVE_AVX512_FLAGS})
            STRING(TOUPPER FLAG_UPPER ${FLAG})
            ADD_CXX_COMPILER_FLAG_IF_AVAILABLE("-mno-${FLAG} " HAVE_NO_${FLAG_UPPER})
        endforeach ()
    endif ()
endif()

mark_as_advanced(HAVE_SSE, HAVE_AVX, HAVE_AVX2, HAVE_FMA, HAVE_AVX512)
