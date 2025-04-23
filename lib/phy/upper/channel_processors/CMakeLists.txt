#
# Copyright 2021-2025 Software Radio Systems Limited
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

add_subdirectory(pdcch)
add_subdirectory(pdsch)
add_subdirectory(pucch)
add_subdirectory(pusch)
add_subdirectory(ssb)
add_subdirectory(uci)

add_library(srsran_prach_detector STATIC
        prach_detector_generic_impl.cpp
        prach_detector_generic_thresholds.cpp
        prach_generator_impl.cpp
        prach_detector_phy_validator_impl.cpp)
target_link_libraries(srsran_prach_detector srsran_ran srsvec)

add_library(srsran_channel_processors STATIC channel_processor_factories.cpp)
target_link_libraries(srsran_channel_processors
        srsran_channel_coding
        srsran_channel_modulation
        srsran_channel_precoder
        srsran_pdcch_processor
        srsran_pdsch_processor
        srsran_phy_support
        srsran_prach_detector
        srsran_pucch_demodulator
        srsran_pucch_detector
        srsran_pucch_processor
        srsran_pusch_processor
        srsran_sequence_generators
        srsran_signal_processors
        srsran_ssb_tx_processor
        srsran_uci_decoder)

add_to_exported_libs(srsran_channel_processors
        srsran_pdcch_processor
        srsran_pdsch_processor
        srsran_prach_detector
        srsran_pucch_demodulator
        srsran_pucch_detector
        srsran_pucch_processor
        srsran_pusch_processor
        srsran_ssb_tx_processor
        srsran_uci_decoder)
