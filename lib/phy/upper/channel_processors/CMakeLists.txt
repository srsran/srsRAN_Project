#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

include_directories(../../../../include/srsgnb/phy/upper/channel_processors)

add_library(srsgnb_pbch_encoder STATIC pbch_encoder_impl.cpp)
target_link_libraries(srsgnb_pbch_encoder srsgnb_channel_coding srsgnb_sequence_generators srsvec)

add_library(srsgnb_pbch_modulator STATIC pbch_modulator_impl.cpp)
target_link_libraries(srsgnb_pbch_modulator srsgnb_channel_modulation srsgnb_sequence_generators srsvec)

add_library(srsgnb_pdcch_encoder STATIC pdcch_encoder_impl.cpp)
target_link_libraries(srsgnb_pdcch_encoder srsgnb_channel_coding srsvec)

add_library(srsgnb_pdcch_modulator STATIC pdcch_modulator_impl.cpp)
target_link_libraries(srsgnb_pdcch_modulator srsvec)

add_library(srsgnb_pdcch_processor STATIC pdcch_processor_impl.cpp)
target_link_libraries(srsgnb_pdcch_processor srsgnb_ran)

add_library(srsgnb_pdsch_encoder STATIC pdsch_encoder_impl.cpp)
target_link_libraries(srsgnb_pdsch_encoder srsgnb_channel_coding)

add_library(srsgnb_pdsch_modulator STATIC pdsch_modulator_impl.cpp)
target_link_libraries(srsgnb_pdsch_modulator srsgnb_channel_modulation srsgnb_sequence_generators srsvec srsgnb_upper_phy_support)

add_library(srsgnb_pdsch_processor STATIC pdsch_processor_impl.cpp)
target_link_libraries(srsgnb_pdsch_processor srsgnb_upper_phy_support)

add_library(srsgnb_prach_detector STATIC
        prach_detector_simple_impl.cpp
        prach_generator_impl.cpp)
target_link_libraries(srsgnb_prach_detector srsgnb_ran srsvec)

add_library(srsgnb_pucch_processor STATIC pucch_processor_impl.cpp)

add_library(srsgnb_pucch_detector STATIC pucch_detector_impl.cpp)
target_include_directories(srsgnb_pucch_detector
        PUBLIC ${CMAKE_SOURCE_DIR}/include/srsgnb/phy/upper/channel_processors
        )

add_library(srsgnb_pusch_decoder STATIC pusch_decoder_impl.cpp)
target_link_libraries(srsgnb_pusch_decoder srsgnb_channel_coding srsvec)

add_library(srsgnb_pusch_demodulator STATIC pusch_demodulator_impl.cpp)

add_library(srsgnb_pusch_processor STATIC pusch_processor_impl.cpp)
target_link_libraries(srsgnb_pusch_processor srsgnb_upper_phy_support)

add_library(srsgnb_ssb_processor STATIC ssb_processor_impl.cpp)
target_link_libraries(srsgnb_ssb_processor srsgnb_sequence_generators srsgnb_signal_processors)

add_library(srsgnb_uci_decoder STATIC uci_decoder_impl.cpp)
target_link_libraries(srsgnb_uci_decoder srsgnb_channel_coding srsvec)

add_library(srsgnb_ulsch_demux STATIC ulsch_demultiplex_impl.cpp)

add_library(srsgnb_pucch_demodulator STATIC pucch_demodulator_impl.cpp)

add_library(srsgnb_channel_processors STATIC channel_processor_factories.cpp)
target_link_libraries(srsgnb_channel_processors
        srsgnb_pbch_encoder
        srsgnb_pbch_modulator
        srsgnb_pdcch_encoder
        srsgnb_pdcch_modulator
        srsgnb_pdcch_processor
        srsgnb_pdsch_encoder
        srsgnb_pdsch_modulator
        srsgnb_pdsch_processor
        srsgnb_prach_detector
        srsgnb_pucch_demodulator
        srsgnb_pucch_detector
        srsgnb_pucch_processor
        srsgnb_pusch_decoder
        srsgnb_pusch_demodulator
        srsgnb_pusch_processor
        srsgnb_ssb_processor
        srsgnb_uci_decoder
        srsgnb_ulsch_demux)

install(TARGETS srsgnb_channel_processors
        srsgnb_pbch_encoder
        srsgnb_pbch_modulator
        srsgnb_pdcch_encoder
        srsgnb_pdcch_modulator
        srsgnb_pdcch_processor
        srsgnb_pdsch_encoder
        srsgnb_pdsch_modulator
        srsgnb_pdsch_processor
        srsgnb_prach_detector
        srsgnb_pucch_demodulator
        srsgnb_pucch_detector
        srsgnb_pucch_processor
        srsgnb_pusch_decoder
        srsgnb_pusch_demodulator
        srsgnb_pusch_processor
        srsgnb_ssb_processor
        srsgnb_uci_decoder
        srsgnb_ulsch_demux
        EXPORT srsgnb_export)
