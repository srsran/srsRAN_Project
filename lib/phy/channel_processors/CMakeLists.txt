#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        pbch_encoder_impl.cpp
        )

include_directories(${PROJECT_SOURCE_DIR}/include/srsgnb/phy/channel_processors)

add_library(pbch_encoder STATIC ${SOURCES})
