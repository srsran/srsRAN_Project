#
# Copyright 2021-2024 Software Radio Systems Limited
#
# This file is part of srsRAN
#
# srsRAN is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
#
# srsRAN is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# A copy of the GNU Affero General Public License can be found in
# the LICENSE file in the top-level directory of this distribution
# and at http://www.gnu.org/licenses/.
#

add_library(srsran_radio STATIC
        plugin_radio_factory.cpp
        radio_factory.cpp)

set(SRSGNB_RADIO_DEFINITIONS "")
set(SRSGNB_RADIO_LIBRARIES "")

if (UHD_FOUND AND ENABLE_UHD)
    add_subdirectory(uhd)
    list(APPEND SRSGNB_RADIO_DEFINITIONS -DENABLE_UHD)
    list(APPEND SRSGNB_RADIO_LIBRARIES srsran_radio_uhd)
endif (UHD_FOUND AND ENABLE_UHD)

if (ZEROMQ_FOUND AND ENABLE_ZEROMQ)
    add_subdirectory(zmq)
    list(APPEND SRSGNB_RADIO_DEFINITIONS -DENABLE_ZMQ)
    list(APPEND SRSGNB_RADIO_LIBRARIES srsran_radio_zmq)
endif (ZEROMQ_FOUND AND ENABLE_ZEROMQ)

add_definitions(${SRSGNB_RADIO_DEFINITIONS})
target_link_libraries(srsran_radio ${SRSGNB_RADIO_LIBRARIES} srslog dl)

install(TARGETS srsran_radio EXPORT srsran_export)