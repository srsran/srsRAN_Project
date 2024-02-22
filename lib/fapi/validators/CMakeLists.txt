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

set(SOURCES
        dl_csi_pdu.cpp
        dl_pdcch_pdu.cpp
        dl_pdsch_pdu.cpp
        dl_ssb_pdu.cpp
        message_validators.cpp
        tx_precoding_and_beamforming_pdu.cpp
        uci_pdus.cpp
        ul_prach_pdu.cpp
        ul_pucch_pdu.cpp
        ul_pusch_pdu.cpp)

add_library(srsran_fapi_validators STATIC ${SOURCES})
target_link_libraries(srsran_fapi_validators srslog)
