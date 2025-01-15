#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(cu_cp)
add_subdirectory(e2)

set(SOURCES
        o_cu_cp_application_unit_impl.cpp
        o_cu_cp_builder.cpp
        o_cu_cp_unit_impl.cpp)

add_library(srsran_o_cu_cp_app_unit STATIC ${SOURCES})
target_include_directories(srsran_o_cu_cp_app_unit PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(srsran_o_cu_cp_app_unit srsran_o_cu_cp srsran_cu_cp_app_unit srsran_o_cu_cp_e2_app_unit)
