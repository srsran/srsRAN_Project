#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(messages)

add_library(srsran_mac_to_fapi_p7_fastpath_translator STATIC mac_to_fapi_fastpath_translator.cpp)
target_link_libraries(srsran_mac_to_fapi_p7_fastpath_translator PRIVATE srsran_mac_fapi_adaptors
                                                                        srsran_fapi_precoding_matrix_tools
                                                                        srsran_fapi)


set(SOURCES
        fapi_to_mac_data_msg_fastpath_translator.cpp
        fapi_to_mac_error_msg_fastpath_translator.cpp
        fapi_to_mac_time_msg_fastpath_translator.cpp)

add_library(srsran_fapi_to_mac_p7_fastpath_translator STATIC ${SOURCES})
target_link_libraries(srsran_fapi_to_mac_p7_fastpath_translator PRIVATE srsvec srsran_support)

add_library(srsran_mac_to_fapi_p7_sector_fastpath_adaptor STATIC mac_fapi_p7_sector_fastpath_adaptor_impl.cpp
                                                                 mac_fapi_p7_sector_fastpath_adaptor_factory_impl.cpp)
target_link_libraries(srsran_mac_to_fapi_p7_sector_fastpath_adaptor PRIVATE srsran_mac_to_fapi_p7_fastpath_translator
                                                                            srsran_fapi_to_mac_p7_fastpath_translator)
