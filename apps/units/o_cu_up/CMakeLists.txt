#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(cu_up)
add_subdirectory(e2)

set(SOURCES
        o_cu_up_application_unit_impl.cpp
        o_cu_up_builder.cpp
        o_cu_up_unit_impl.cpp)

add_library(srsran_o_cu_up_app_unit STATIC ${SOURCES})
target_include_directories(srsran_o_cu_up_app_unit PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(srsran_o_cu_up_app_unit srsran_cu_up_app_unit srsran_o_cu_up_app_unit_e2 srsran_o_cu_up)
