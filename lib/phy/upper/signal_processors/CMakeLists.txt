#
# Copyright 2021-2024 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        ptrs/ptrs_pdsch_generator_impl.cpp
        ptrs/ptrs_pdsch_generator_factory.cpp
        pucch/dmrs_pucch_estimator_format1.cpp
        pucch/dmrs_pucch_estimator_format2.cpp
        pucch/dmrs_pucch_estimator_formats_3_4.cpp
        srs/srs_estimator_factory.cpp
        srs/srs_estimator_generic_impl.cpp
        srs/srs_validator_generic_impl.cpp
        dmrs_pbch_processor_impl.cpp
        dmrs_pdcch_processor_impl.cpp
        dmrs_pdsch_processor_impl.cpp
        dmrs_pusch_estimator_impl.cpp
        nzp_csi_rs_generator_impl.cpp
        port_channel_estimator_average_impl.cpp
        port_channel_estimator_helpers.cpp
        signal_processor_factories.cpp
        pss_processor_impl.cpp
        sss_processor_impl.cpp
)

add_library(srsran_signal_processors STATIC ${SOURCES})
target_link_libraries(srsran_signal_processors srsvec srsran_phy_support srsran_upper_phy_support srsran_ran srsran_generic_funcs)

add_to_exported_libs(srsran_signal_processors)
