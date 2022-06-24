#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
  rlc_am_pdu.cpp
  rlc_factory.cpp
  rlc_rx_tm_entity.cpp
  rlc_rx_um_entity.cpp
)

add_library(srsgnb_rlc STATIC ${SOURCES})
target_link_libraries(srsgnb_rlc)
