#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(messages)

add_library(srsran_fapi_to_phy_translator STATIC fapi_to_phy_translator.cpp)
target_link_libraries(srsran_fapi_to_phy_translator
        srsran_fapi_phy_message_adaptors
        srsran_fapi_precoding_matrix_tools
        srsran_instrumentation)

set(SOURCES
        phy_to_fapi_error_event_translator.cpp
        phy_to_fapi_results_event_translator.cpp
        phy_to_fapi_time_event_translator.cpp)

add_library(srsran_phy_to_fapi_translator STATIC ${SOURCES})
target_link_libraries(srsran_phy_to_fapi_translator srslog)

add_library(srsran_phy_fapi_adaptor STATIC phy_fapi_sector_adaptor_impl.cpp phy_fapi_adaptor_impl.cpp phy_fapi_adaptor_factory_impl.cpp)
target_link_libraries(srsran_phy_fapi_adaptor srsran_fapi_to_phy_translator srsran_phy_to_fapi_translator)
