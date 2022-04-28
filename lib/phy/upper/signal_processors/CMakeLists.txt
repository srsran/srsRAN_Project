#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        dmrs_pbch_processor_impl.cpp
        dmrs_pdcch_processor_impl.cpp
        dmrs_pdsch_processor_impl.cpp
        pucch/dmrs_pucch_processor_format1_impl.cpp
        pucch/dmrs_pucch_processor_format2_impl.cpp
        dmrs_pucch_processor_impl.cpp
        pss_processor_impl.cpp
        sss_processor_impl.cpp
        )

include_directories(../../../../include/srsgnb/phy/upper/signal_processors)

add_library(signal_processors STATIC ${SOURCES})
