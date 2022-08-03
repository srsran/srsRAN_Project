#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(srsgnb_lower_phy_modulation STATIC
        modulation_factories.cpp
        ofdm_modulator_impl.cpp
        ofdm_demodulator_impl.cpp
        ofdm_prach_demodulator_impl.cpp)
target_link_libraries(srsgnb_lower_phy_modulation srsvec srsgnb_ran)
