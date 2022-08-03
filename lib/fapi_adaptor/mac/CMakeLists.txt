# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(messages)

add_library(srsgnb_mac_to_fapi_translator STATIC mac_to_fapi_translator.cpp)
target_link_libraries(srsgnb_mac_to_fapi_translator srsgnb_mac_fapi_adaptors fapi_validators)


set(SOURCES
        fapi_to_mac_data_msg_translator.cpp
        fapi_to_mac_time_msg_translator.cpp)

add_library(srsgnb_fapi_to_mac_translator STATIC ${SOURCES})

add_library(mac_fapi_adaptor STATIC mac_fapi_adaptor_impl.cpp)
target_link_libraries(mac_fapi_adaptor srsgnb_mac_to_fapi_translator srsgnb_fapi_to_mac_translator)

add_library(mac_fapi_adaptor_factory STATIC mac_fapi_adaptor_factory_impl.cpp)
target_link_libraries(mac_fapi_adaptor_factory mac_fapi_adaptor)
