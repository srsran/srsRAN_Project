#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        channel_equalizer_zf_impl.cpp
        equalization_factories.cpp
        equalize_zf_1xn.cpp
        equalize_zf_2x2.cpp
        )

add_library(srsgnb_channel_equalizer STATIC ${SOURCES})
target_link_libraries(srsgnb_channel_equalizer srsvec)

install(TARGETS srsgnb_channel_equalizer EXPORT srsgnb_export)