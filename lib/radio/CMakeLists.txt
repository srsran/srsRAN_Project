
add_library(srsgnb_radio STATIC radio_factory.cpp)

set(SRSGNB_RADIO_DEFINITIONS "")
set(SRSGNB_RADIO_LIBRARIES "")

if (UHD_FOUND AND ENABLE_UHD)
    add_subdirectory(uhd)
    list(APPEND SRSGNB_RADIO_DEFINITIONS -DENABLE_UHD)
    list(APPEND SRSGNB_RADIO_LIBRARIES srsgnb_radio_uhd)
endif (UHD_FOUND AND ENABLE_UHD)

add_definitions(${SRSGNB_RADIO_DEFINITIONS})
target_link_libraries(srsgnb_radio ${SRSGNB_RADIO_LIBRARIES})