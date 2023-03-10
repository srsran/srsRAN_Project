#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        dl_csi_pdu.cpp
        dl_pdcch_pdu.cpp
        dl_pdsch_pdu.cpp
        dl_ssb_pdu.cpp
        uci_pdus.cpp
        ul_prach_pdu.cpp
        ul_pucch_pdu.cpp
        ul_pusch_pdu.cpp)

add_library(srsran_fapi_pdu_validators STATIC ${SOURCES})
target_link_libraries(srsran_fapi_pdu_validators srslog)
