#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(du_high)
add_subdirectory(e2)
add_subdirectory(fapi)

add_library(srsran_o_du_high_unit_helpers STATIC o_du_high_unit_factory.cpp)
target_include_directories(srsran_o_du_high_unit_helpers PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(srsran_o_du_high_unit_helpers PRIVATE
        srsran_du_high_unit_helpers
        srsran_fapi_app_unit
        srsran_o_du_high_app_unit_e2
        srsran_o_du_high)
