#
# Copyright 2013-2022 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
  rrc_factory.cpp
  ue/rrc_ue_entity.cpp
  ue/rrc_ue_message_handlers.cpp
  ue/rrc_ue_message_senders.cpp
)

add_library(srsgnb_rrc STATIC ${SOURCES})
target_link_libraries(srsgnb_rrc rrc_nr_asn1)
