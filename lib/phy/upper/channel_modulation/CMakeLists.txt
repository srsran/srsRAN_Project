#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        modulation_mapper_impl.cpp
        demodulation_mapper_impl.cpp
        )

include_directories(../../../../include/srsgnb/phy/upper/channel_modulation)

add_library(modulation_mapper STATIC ${SOURCES})
target_link_libraries(modulation_mapper srsgnb_support)
