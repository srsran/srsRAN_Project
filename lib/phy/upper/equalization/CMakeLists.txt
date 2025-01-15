#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        channel_equalizer_generic_impl.cpp
        equalization_factories.cpp)

add_library(srsran_channel_equalizer STATIC ${SOURCES})
target_link_libraries(srsran_channel_equalizer srsvec)

add_to_exported_libs(srsran_channel_equalizer)
