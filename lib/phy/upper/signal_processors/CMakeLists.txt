#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        csi_rs_processor_impl.cpp
        dmrs_pbch_processor_impl.cpp
        dmrs_pdcch_processor_impl.cpp
        dmrs_pdsch_processor_impl.cpp
        pucch/dmrs_pucch_processor_format1_impl.cpp
        pucch/dmrs_pucch_processor_format2_impl.cpp
        pucch/dmrs_pucch_processor_factories.cpp
        signal_processor_factories.cpp
        pss_processor_impl.cpp
        sss_processor_impl.cpp
        )

include_directories(../../../../include/srsgnb/phy/upper/signal_processors)

add_library(srsgnb_signal_processors STATIC ${SOURCES})

install(TARGETS srsgnb_signal_processors EXPORT srsgnb_export)
