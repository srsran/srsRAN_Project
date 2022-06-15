#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(messages)

set(SOURCES
        phy_to_fapi_translator.cpp
        slot_event_dispatcher.cpp)

add_library(phy_to_fapi_translator STATIC ${SOURCES})
target_link_libraries(phy_to_fapi_translator srslog)

add_library(pdsch_fapi_adaptor STATIC pdsch.cpp)
target_link_libraries(pdsch_fapi_adaptor)

add_library(ssb_fapi_adaptor STATIC ssb.cpp)
target_link_libraries(ssb_fapi_adaptor srsvec)
