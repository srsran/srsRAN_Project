#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        pdcch.cpp
        pdsch.cpp
        prach.cpp
        pusch.cpp
        ssb.cpp)

add_library(srsgnb_fapi_phy_message_adaptors STATIC ${SOURCES})
target_link_libraries(srsgnb_fapi_phy_message_adaptors srsvec srsgnb_upper_phy_support)
