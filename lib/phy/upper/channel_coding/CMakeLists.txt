#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

include_directories(${PROJECT_SOURCE_DIR}/include/srsgnb/phy/upper/channel_coding)

set(CRC_CALCULATOR_SOURCES crc_calculator_lut_impl.cpp)
if (${HAVE_AVX2})
    list(APPEND CRC_CALCULATOR_SOURCES crc_calculator_clmul_impl.cpp)
    set_source_files_properties(crc_calculator_clmul_impl.cpp PROPERTIES COMPILE_OPTIONS "-mpclmul;")
endif (${HAVE_AVX2})


add_library(srsgnb_crc_calculator STATIC ${CRC_CALCULATOR_SOURCES})

add_subdirectory(polar)
add_subdirectory(ldpc)
add_subdirectory(short)

add_library(srsgnb_channel_coding STATIC channel_coding_factories.cpp)
target_link_libraries(srsgnb_channel_coding
        srsgnb_crc_calculator
        srsgnb_polar
        srsgnb_ldpc
        srsgnb_short_block)

install(TARGETS srsgnb_crc_calculator
        srsgnb_channel_coding
        EXPORT srsgnb_export)
