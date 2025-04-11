#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

add_library(srsran_ssb_tx_processor STATIC
        factories.cpp
        pbch_encoder_impl.cpp
        pbch_modulator_impl.cpp
        ssb_processor_impl.cpp)
target_link_libraries(srsran_ssb_tx_processor
        srsran_sequence_generators
        srsran_signal_processors)