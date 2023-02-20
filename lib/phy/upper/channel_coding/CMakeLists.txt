#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

include_directories(${PROJECT_SOURCE_DIR}/include/srsgnb/phy/upper/channel_coding)

set(CRC_CALCULATOR_SOURCES crc_calculator_lut_impl.cpp)
if (${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")
    list(APPEND CRC_CALCULATOR_SOURCES crc_calculator_clmul_impl.cpp)
    set_source_files_properties(crc_calculator_clmul_impl.cpp PROPERTIES COMPILE_OPTIONS "-mpclmul;-msse4.1")
endif (${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64")


add_library(srsran_crc_calculator STATIC ${CRC_CALCULATOR_SOURCES})

add_subdirectory(polar)
add_subdirectory(ldpc)
add_subdirectory(short)

add_library(srsran_channel_coding STATIC channel_coding_factories.cpp)

target_link_libraries(srsran_channel_coding
        srsran_crc_calculator
        srsran_polar
        srsran_ldpc
        srsran_short_block)