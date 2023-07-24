#
# Copyright 2021-2023 Software Radio Systems Limited
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

set(SOURCES
  rrc_du_factory.cpp
  rrc_du_impl.cpp
  ue/meas_types_validators.cpp
  ue/rrc_ue_impl.cpp
  ue/rrc_ue_message_handlers.cpp
  ue/rrc_ue_message_senders.cpp
  ue/rrc_ue_helpers.cpp
  ue/procedures/rrc_setup_procedure.cpp
  ue/procedures/rrc_security_mode_command_procedure.cpp
  ue/procedures/rrc_reconfiguration_procedure.cpp
  ue/procedures/rrc_ue_capability_transfer_procedure.cpp
  ue/procedures/rrc_reestablishment_procedure.cpp
)

add_library(srsran_rrc STATIC ${SOURCES})
target_link_libraries(srsran_rrc srsran_up_resource_manager srsran_security rrc_nr_asn1)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})