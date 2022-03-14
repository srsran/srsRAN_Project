#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

include_directories(../../../../include/srsgnb/phy/upper/channel_processors)

add_library(pbch_encoder STATIC pbch_encoder_impl.cpp)
target_link_libraries(pbch_encoder polar pseudo_random_generator crc_calculator srsvec)

add_library(pbch_modulator STATIC pbch_modulator_impl.cpp)
target_link_libraries(pbch_modulator modulation_mapper pseudo_random_generator srsvec)

add_library(pdsch_modulator STATIC pdsch_modulator_impl.cpp)
target_link_libraries(pdsch_modulator modulation_mapper pseudo_random_generator srsvec upper_phy)

add_library(ssb_processor STATIC ssb_processor_impl.cpp)
target_link_libraries(ssb_processor pbch_encoder pbch_modulator signal_processors)
