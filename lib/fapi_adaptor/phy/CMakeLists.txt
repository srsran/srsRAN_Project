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

add_library(fapi_to_phy_translator STATIC fapi_to_phy_translator.cpp)
target_link_libraries(fapi_to_phy_translator srsgnb_fapi_phy_message_adaptors)

add_library(phy_to_fapi_translator STATIC ${SOURCES})
target_link_libraries(phy_to_fapi_translator srslog)

add_library(phy_fapi_adaptor STATIC phy_fapi_adaptor_impl.cpp)
target_link_libraries(phy_fapi_adaptor fapi_to_phy_translator phy_to_fapi_translator)
