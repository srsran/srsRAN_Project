#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        pdcch.cpp
        pdsch.cpp
        prach.cpp
        pucch.cpp
        pusch.cpp
        ssb.cpp)

add_library(srsran_mac_fapi_adaptors STATIC ${SOURCES})
target_link_libraries(srsran_mac_fapi_adaptors srslog)
