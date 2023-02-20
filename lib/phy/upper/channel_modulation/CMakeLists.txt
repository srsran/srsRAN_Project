#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
        channel_modulation_factories.cpp
        demodulation_mapper_impl.cpp
        demodulation_mapper_qam16.cpp
        demodulation_mapper_qam64.cpp
        demodulation_mapper_qam256.cpp
        demodulation_mapper_qpsk.cpp
        evm_calculator_generic_impl.cpp
        modulation_mapper_impl.cpp)

include_directories(../../../../include/srsgnb/phy/upper/channel_modulation)

add_library(srsran_channel_modulation STATIC ${SOURCES})
target_link_libraries(srsran_channel_modulation srsran_support log_likelihood_ratio)