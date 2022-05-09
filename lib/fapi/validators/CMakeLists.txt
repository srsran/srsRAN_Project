#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        dl_pdcch_pdu.cpp
        dl_ssb_pdu.cpp
        message_validators.cpp)

add_library(fapi_validators STATIC ${SOURCES})
