#
# Copyright 2021-2023 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

set(SOURCES
  rrc_du_factory.cpp
  rrc_du_impl.cpp
  ue/rrc_ue_impl.cpp
  ue/rrc_ue_message_handlers.cpp
  ue/rrc_ue_message_senders.cpp
  ue/rrc_ue_helpers.cpp
  ue/drb_manager_impl.cpp
  ue/procedures/rrc_setup_procedure.cpp
  ue/procedures/rrc_security_mode_command_procedure.cpp
  ue/procedures/rrc_reconfiguration_procedure.cpp
)

add_library(srsgnb_rrc STATIC ${SOURCES})
target_link_libraries(srsgnb_rrc srsgnb_security rrc_nr_asn1)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})