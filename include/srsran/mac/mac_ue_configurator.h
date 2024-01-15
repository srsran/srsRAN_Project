/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "mac_cell_group_config.h"
#include "srsran/mac/bsr_format.h"
#include "srsran/mac/mac_sdu_handler.h"
#include "srsran/mac/phr_config.h"
#include "srsran/mac/time_alignment_group_config.h"
#include "srsran/ran/lcid.h"
#include "srsran/ran/physical_cell_group.h"
#include "srsran/ran/rnti.h"
#include "srsran/ran/sr_configuration.h"
#include "srsran/scheduler/scheduler_configurator.h"
#include "srsran/support/async/async_task.h"

namespace srsran {

class unique_timer;

/// Interface used to notify detected radio link failures in the MAC (e.g. due to max KOs reached) for a given UE.
class mac_ue_radio_link_notifier
{
public:
  virtual ~mac_ue_radio_link_notifier() = default;

  /// \brief Notifies that a radio link failure has been detected for a given UE.
  virtual void on_rlf_detected() = 0;

  /// \brief Notifies that a MAC C-RNTI CE was received with old C-RNTI set to equal to the given UE.
  ///
  /// The detection of a MAC C-RNTI CE should cancel the handling of any previously detected RLF due to out-of-sync
  /// (e.g. PUCCH/PUSCH KOs). It should not have an effect on RLFs due the RLC max retransmissions or protocol failure
  /// as those problems are not possible to recover from without Reestablishment.
  virtual void on_crnti_ce_received() = 0;
};

/// Parameters passed to MAC concerning a created logical channel.
struct mac_logical_channel_config {
  lcid_t               lcid;
  mac_sdu_rx_notifier* ul_bearer;
  mac_sdu_tx_builder*  dl_bearer;
};

/// Input parameters used to create a UE in the scheduler.
struct mac_ue_create_request {
  /// Serving cell of the UE to be created.
  du_cell_index_t cell_index;
  /// \brief DU-specific UE index for the UE to be created.
  du_ue_index_t ue_index;
  /// \brief C-RNTI to assign to the newly created UE. The C-RNTI can be set to be equal to a previously allocated
  /// TC-RNTI, in case the UE went through RA procedure. If this field is equal INVALID_RNTI, the MAC will generate a
  /// new C-RNTI.
  rnti_t                                  crnti;
  mac_ue_radio_link_notifier*             rlf_notifier;
  std::vector<mac_logical_channel_config> bearers;
  mac_cell_group_config                   mac_cell_group_cfg;
  physical_cell_group_config              phy_cell_group_cfg;
  const byte_buffer*                      ul_ccch_msg;
  // Scheduler-only params.
  sched_ue_config_request sched_cfg;
};

/// Outcome of a MAC UE creation request procedure.
struct mac_ue_create_response {
  du_cell_index_t cell_index = INVALID_DU_CELL_INDEX;
  du_ue_index_t   ue_index   = INVALID_DU_UE_INDEX;
  /// C-RNTI allocated to the created UE in the MAC. INVALID_RNTI if the UE was not created.
  rnti_t allocated_crnti = rnti_t::INVALID_RNTI;
};

/// Input parameters used to reconfigure a UE in the scheduler.
struct mac_ue_reconfiguration_request {
  du_ue_index_t                           ue_index;
  du_cell_index_t                         pcell_index;
  rnti_t                                  crnti;
  std::vector<mac_logical_channel_config> bearers_to_addmod;
  std::vector<lcid_t>                     bearers_to_rem;
  optional<mac_cell_group_config>         mac_cell_group_cfg;
  optional<physical_cell_group_config>    phy_cell_group_cfg;
  // Scheduler-only params.
  sched_ue_config_request sched_cfg;
};

/// \brief Outcome of a MAC UE reconfiguration request procedure.
struct mac_ue_reconfiguration_response {
  du_ue_index_t ue_index;
  bool          result;
};

/// Input parameters used to delete a UE in the scheduler.
struct mac_ue_delete_request {
  du_cell_index_t cell_index;
  du_ue_index_t   ue_index;
  rnti_t          rnti;
};

/// \brief Outcome of a MAC UE deletion request procedure.
struct mac_ue_delete_response {
  bool result;
};

/// \brief Interface used to manage the creation, reconfiguration and deletion of UEs in MAC
class mac_ue_configurator
{
public:
  virtual ~mac_ue_configurator() = default;

  /// \brief Creates a UE context in the MAC.
  virtual async_task<mac_ue_create_response> handle_ue_create_request(const mac_ue_create_request& cfg) = 0;

  /// \brief Reconfigures an existing UE context in the MAC.
  virtual async_task<mac_ue_reconfiguration_response>
  handle_ue_reconfiguration_request(const mac_ue_reconfiguration_request& cfg) = 0;

  /// \brief Deletes an existing UE context in the MAC.
  virtual async_task<mac_ue_delete_response> handle_ue_delete_request(const mac_ue_delete_request& cfg) = 0;

  /// \brief Forward UL-CCCH message to upper layers.
  virtual bool handle_ul_ccch_msg(du_ue_index_t ue_index, byte_buffer pdu) = 0;
};

} // namespace srsran
