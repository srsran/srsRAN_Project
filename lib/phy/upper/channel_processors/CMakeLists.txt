#
# Copyright 2021-2023 Software Radio Systems Limited
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

add_library(srsran_pbch_encoder STATIC pbch_encoder_impl.cpp)
target_link_libraries(srsran_pbch_encoder srsran_channel_coding srsran_sequence_generators srsvec)

add_library(srsran_pbch_modulator STATIC pbch_modulator_impl.cpp)
target_link_libraries(srsran_pbch_modulator srsran_channel_modulation srsran_sequence_generators srsvec)

add_library(srsran_pdcch_encoder STATIC pdcch_encoder_impl.cpp)
target_link_libraries(srsran_pdcch_encoder srsran_channel_coding srsvec)

add_library(srsran_pdcch_modulator STATIC pdcch_modulator_impl.cpp)
target_link_libraries(srsran_pdcch_modulator srsvec)

add_library(srsran_pdcch_processor STATIC pdcch_processor_impl.cpp)
target_link_libraries(srsran_pdcch_processor srsran_ran)

add_library(srsran_pdsch_encoder STATIC pdsch_encoder_impl.cpp)
target_link_libraries(srsran_pdsch_encoder srsran_channel_coding)

add_library(srsran_pdsch_modulator STATIC pdsch_modulator_impl.cpp)
target_link_libraries(srsran_pdsch_modulator srsran_channel_modulation srsran_sequence_generators srsvec srsran_upper_phy_support)

add_library(srsran_pdsch_processor STATIC
        pdsch_codeblock_processor.cpp
        pdsch_processor_concurrent_impl.cpp
        pdsch_processor_impl.cpp
        pdsch_processor_lite_impl.cpp)
target_link_libraries(srsran_pdsch_processor srsran_upper_phy_support)

add_library(srsran_prach_detector STATIC
        prach_detector_generic_impl.cpp
        prach_generator_impl.cpp)
target_link_libraries(srsran_prach_detector srsran_ran srsvec)

add_library(srsran_pucch_processor STATIC pucch_processor_impl.cpp)

add_library(srsran_pucch_detector STATIC pucch_detector_impl.cpp)

add_library(srsran_pusch_decoder STATIC pusch_decoder_impl.cpp)
target_link_libraries(srsran_pusch_decoder srsran_channel_coding srsvec)

add_library(srsran_pusch_demodulator STATIC pusch_demodulator_impl.cpp)

add_library(srsran_pusch_processor STATIC pusch_processor_impl.cpp)
target_link_libraries(srsran_pusch_processor srsran_upper_phy_support srsran_ran)

add_library(srsran_ssb_processor STATIC ssb_processor_impl.cpp)
target_link_libraries(srsran_ssb_processor srsran_sequence_generators srsran_signal_processors)

add_library(srsran_uci_decoder STATIC uci_decoder_impl.cpp)
target_link_libraries(srsran_uci_decoder srsran_channel_coding srsvec)

add_library(srsran_ulsch_demux STATIC ulsch_demultiplex_impl.cpp)

add_library(srsran_pucch_demodulator STATIC pucch_demodulator_impl.cpp)

add_library(srsran_channel_processors STATIC channel_processor_factories.cpp)
target_link_libraries(srsran_channel_processors
        srsran_pbch_encoder
        srsran_pbch_modulator
        srsran_pdcch_encoder
        srsran_pdcch_modulator
        srsran_pdcch_processor
        srsran_pdsch_encoder
        srsran_pdsch_modulator
        srsran_pdsch_processor
        srsran_prach_detector
        srsran_pucch_demodulator
        srsran_pucch_detector
        srsran_pucch_processor
        srsran_pusch_decoder
        srsran_pusch_demodulator
        srsran_pusch_processor
        srsran_ssb_processor
        srsran_uci_decoder
        srsran_ulsch_demux)

install(TARGETS srsran_channel_processors
        srsran_pbch_encoder
        srsran_pbch_modulator
        srsran_pdcch_encoder
        srsran_pdcch_modulator
        srsran_pdcch_processor
        srsran_pdsch_encoder
        srsran_pdsch_modulator
        srsran_pdsch_processor
        srsran_prach_detector
        srsran_pucch_demodulator
        srsran_pucch_detector
        srsran_pucch_processor
        srsran_pusch_decoder
        srsran_pusch_demodulator
        srsran_pusch_processor
        srsran_ssb_processor
        srsran_uci_decoder
        srsran_ulsch_demux
        EXPORT srsran_export)