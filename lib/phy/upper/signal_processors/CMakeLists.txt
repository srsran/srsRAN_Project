#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        dmrs_pbch_processor_impl.cpp
        dmrs_pdcch_processor_impl.cpp
        dmrs_pdsch_processor_impl.cpp
        dmrs_pusch_estimator_impl.cpp
        nzp_csi_rs_generator_impl.cpp
        port_channel_estimator_average_impl.cpp
        pucch/dmrs_pucch_processor_format1_impl.cpp
        pucch/dmrs_pucch_processor_format2_impl.cpp
        signal_processor_factories.cpp
        pss_processor_impl.cpp
        sss_processor_impl.cpp
        )

include_directories(../../../../include/srsran/phy/upper/signal_processors)

add_library(srsran_signal_processors STATIC ${SOURCES})
target_link_libraries(srsran_signal_processors srsvec srsran_phy_support srsran_upper_phy_support srsran_ran srsran_generic_funcs)

install(TARGETS srsran_signal_processors
        EXPORT srsran_export)