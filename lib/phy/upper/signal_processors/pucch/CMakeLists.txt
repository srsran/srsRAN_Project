include_directories(../../../../../include/srsgnb/phy/upper/channel_processors)

set(SOURCES dmrs_pucch_processor_format1_impl.cpp
        dmrs_pucch_processor_format2_impl.cpp)

add_library(dmrs_pucch_impl STATIC ${SOURCES})

target_link_libraries(dmrs_pucch_impl sequence_generators srsvec)