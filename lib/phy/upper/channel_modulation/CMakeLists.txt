#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
  channel_modulation_factories.cpp
  modulation_mapper_impl.cpp
  demodulation_mapper_impl.cpp
  demodulation_mapper_qam16.cpp
  demodulation_mapper_qam64.cpp
  demodulation_mapper_qam256.cpp
  demodulation_mapper_qpsk.cpp
  )

include_directories(../../../../include/srsgnb/phy/upper/channel_modulation)

add_library(srsgnb_channel_modulation STATIC ${SOURCES})
target_link_libraries(srsgnb_channel_modulation srsgnb_support log_likelihood_ratio)

install(TARGETS srsgnb_channel_modulation EXPORT srsgnb_export)
