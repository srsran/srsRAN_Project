#
# Copyright 2021-2024 Software Radio Systems Limited
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

install(TARGETS srsran_channel_precoder
        EXPORT srsran_export)
