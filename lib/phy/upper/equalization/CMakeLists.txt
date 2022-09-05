#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(srsgnb_channel_equalizer STATIC
        channel_equalizer_zf_impl.cpp
        equalization_factories.cpp)

target_link_libraries(srsgnb_channel_equalizer srsvec)

install(TARGETS srsgnb_channel_equalizer EXPORT srsgnb_export)