#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        configuration_procedure.cpp
        phy_fapi_p5_sector_operation_request_adaptor.cpp)

add_library(srsran_split6_flexible_o_du_low_unit_phy_fapi_p5_adaptor STATIC ${SOURCES})
target_include_directories(srsran_split6_flexible_o_du_low_unit_phy_fapi_p5_adaptor PRIVATE ${CMAKE_SOURCE_DIR})
