#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

include_directories(../../../../include/srsgnb/phy/upper/channel_processors)

add_library(pbch_encoder STATIC pbch_encoder_impl.cpp)
target_link_libraries(pbch_encoder polar sequence_generators crc_calculator srsvec)

add_library(pbch_modulator STATIC pbch_modulator_impl.cpp)
target_link_libraries(pbch_modulator modulation_mapper sequence_generators srsvec)

add_library(pdcch_modulator STATIC pdcch_modulator_impl.cpp)
target_link_libraries(pdcch_modulator srsvec)

add_library(pdcch_processor STATIC pdcch_processor_impl.cpp)
target_link_libraries(pdcch_processor srsgnb_ran)

add_library(pdcch_encoder STATIC pdcch_encoder_impl.cpp)
target_link_libraries(pdcch_encoder polar crc_calculator srsvec)

add_library(srsgnb_pdsch_encoder STATIC pdsch_encoder_impl.cpp)
target_link_libraries(srsgnb_pdsch_encoder ldpc)

add_library(srsgnb_pdsch_modulator STATIC pdsch_modulator_impl.cpp)
target_link_libraries(srsgnb_pdsch_modulator modulation_mapper sequence_generators srsvec srsgnb_upper_phy_support)

add_library(srsgnb_pdsch_processor STATIC pdsch_processor_impl.cpp)
target_link_libraries(srsgnb_pdsch_processor srsgnb_upper_phy_support)

add_library(ssb_processor STATIC ssb_processor_impl.cpp)
target_link_libraries(ssb_processor pbch_encoder pbch_modulator signal_processors)