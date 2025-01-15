#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_subdirectory(pdcch)
add_subdirectory(pdsch)
add_subdirectory(pusch)
add_subdirectory(pucch)
add_subdirectory(uci)

add_library(srsran_pbch_encoder STATIC pbch_encoder_impl.cpp)
target_link_libraries(srsran_pbch_encoder srsran_channel_coding srsran_sequence_generators srsvec)

add_library(srsran_pbch_modulator STATIC pbch_modulator_impl.cpp)
target_link_libraries(srsran_pbch_modulator srsran_channel_modulation srsran_sequence_generators srsvec)

add_library(srsran_prach_detector STATIC
        prach_detector_generic_impl.cpp
        prach_detector_generic_thresholds.cpp
        prach_generator_impl.cpp
        prach_detector_phy_validator_impl.cpp)
target_link_libraries(srsran_prach_detector srsran_ran srsvec)

add_library(srsran_ssb_processor STATIC ssb_processor_impl.cpp)
target_link_libraries(srsran_ssb_processor srsran_sequence_generators srsran_signal_processors)

add_library(srsran_channel_processors STATIC channel_processor_factories.cpp)
target_link_libraries(srsran_channel_processors
        srsran_pbch_encoder
        srsran_pbch_modulator
        srsran_pdcch_processor
        srsran_pdsch_processor
        srsran_prach_detector
        srsran_pucch_demodulator
        srsran_pucch_detector
        srsran_pucch_processor
        srsran_pusch_processor
        srsran_ssb_processor
        srsran_uci_decoder)

add_to_exported_libs(srsran_channel_processors
        srsran_pbch_encoder
        srsran_pbch_modulator
        srsran_pdcch_processor
        srsran_pdsch_processor
        srsran_prach_detector
        srsran_pucch_demodulator
        srsran_pucch_detector
        srsran_pucch_processor
        srsran_pusch_processor
        srsran_ssb_processor
        srsran_uci_decoder)
