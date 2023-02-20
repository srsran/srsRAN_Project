#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(srsgnb_radio STATIC radio_factory.cpp)

set(SRSGNB_RADIO_DEFINITIONS "")
set(SRSGNB_RADIO_LIBRARIES "")

if (UHD_FOUND AND ENABLE_UHD)
    add_subdirectory(uhd)
    list(APPEND SRSGNB_RADIO_DEFINITIONS -DENABLE_UHD)
    list(APPEND SRSGNB_RADIO_LIBRARIES srsgnb_radio_uhd)
endif (UHD_FOUND AND ENABLE_UHD)

if (ZEROMQ_FOUND AND ENABLE_ZEROMQ)
    add_subdirectory(zmq)
    list(APPEND SRSGNB_RADIO_DEFINITIONS -DENABLE_ZMQ)
    list(APPEND SRSGNB_RADIO_LIBRARIES srsgnb_radio_zmq)
endif (ZEROMQ_FOUND AND ENABLE_ZEROMQ)

add_definitions(${SRSGNB_RADIO_DEFINITIONS})
target_link_libraries(srsgnb_radio ${SRSGNB_RADIO_LIBRARIES} srslog)

install(TARGETS srsgnb_radio EXPORT srsgnb_export)