#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(channel_precoder_sources
        channel_precoder_impl.cpp
        channel_precoder_generic.cpp
        precoding_factories.cpp)

if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
    list(APPEND channel_precoder_sources
            channel_precoder_avx2.cpp
            channel_precoder_avx512.cpp)
    set_source_files_properties(channel_precoder_avx2.cpp PROPERTIES COMPILE_OPTIONS "-mavx2;-mfma;")
    set_source_files_properties(channel_precoder_avx512.cpp PROPERTIES COMPILE_OPTIONS "-mavx512f;-mavx512bw")
endif (${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")

if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64")
    list(APPEND channel_precoder_sources channel_precoder_neon.cpp)
endif (${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64")


add_library(srsran_channel_precoder STATIC ${channel_precoder_sources})

add_to_exported_libs(srsran_channel_precoder)
