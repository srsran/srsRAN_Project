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

#include "../ran_resource_management/cell_group_config.h"
#include "../ran_resource_management/du_ran_resource_manager.h"
#include "du_ue_bearer_manager.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/rnti.h"

namespace srsran {
namespace srs_du {

/// \brief This class holds the context of an UE in the DU.
struct du_ue_context {
  du_ue_context(du_ue_index_t ue_index_, du_cell_index_t pcell_index_, rnti_t rnti_) :
    ue_index(ue_index_), rnti(rnti_), f1ap_ue_id(int_to_gnb_du_ue_f1ap_id(ue_index_)), pcell_index(pcell_index_)
  {
  }

  const du_ue_index_t ue_index;
  rnti_t              rnti = rnti_t::INVALID_RNTI;
  gnb_du_ue_f1ap_id_t f1ap_ue_id;
  du_cell_index_t     pcell_index;
};

/// The interface exposes the methods to interact with the state of a DU UE.
class du_ue_controller
{
public:
  virtual ~du_ue_controller() = default;

  /// \brief Disconnect the UE inter-layer notifiers.
  ///
  /// This method should be called as a first step in the deletion of a UE, to ensure traffic is not flowing through
  /// its bearers during the layer by layer UE context removal.
  virtual async_task<void> disconnect_notifiers() = 0;

  /// \brief Schedule task for a given UE.
  virtual void schedule_async_task(async_task<void> task) = 0;

  /// \brief Handle detection of RLF in MAC or RLC.
  virtual void handle_rlf_detection(rlf_cause cause) = 0;

  /// \brief Handle the detection of a C-RNTI MAC CE for this UE.
  virtual void handle_crnti_ce_detection() = 0;

  /// \brief Stop DRB traffic from flowing through the stack.
  ///
  /// This method may be called when Radio Link Failures are detected.
  virtual void stop_drb_traffic() = 0;

  /// \brief Access to the MAC RLF notifier for this UE.
  virtual mac_ue_radio_link_notifier& get_mac_rlf_notifier() = 0;

  /// \brief Access to the RLC RLF notifier for this UE.
  virtual rlc_tx_upper_layer_control_notifier& get_rlc_rlf_notifier() = 0;
};

class du_ue : public du_ue_context, public du_ue_controller
{
public:
  explicit du_ue(const du_ue_context& ctx_, ue_ran_resource_configurator resources_) :
    du_ue_context(ctx_), resources(std::move(resources_))
  {
  }

  /// \brief List of bearers of the UE.
  du_ue_bearer_manager bearers;

  /// \brief Radio access network resources currently allocated to the UE.
  ue_ran_resource_configurator resources;

  /// \brief Determines whether this UE is running the RRC Reestablishment procedure.
  // TODO: refactor.
  bool reestablishment_pending = false;
};

} // namespace srs_du
} // namespace srsran
