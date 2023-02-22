/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "../../ran/gnb_format.h"
#include "../converters/mac_config_helpers.h"
#include "../du_ue/du_ue.h"
#include "../du_ue/ue_manager_ctrl_configurator.h"
#include "srsran/du_manager/du_manager_params.h"
#include "srsran/mac/mac.h"
#include "srsran/rlc/rlc_factory.h"
#include "srsran/rlc/rlc_rx.h"
#include "srsran/rlc/rlc_tx.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_du {

/// \brief Handles the creation of a UE and respective bearers in the DU UE manager, MAC, F1.
///  \startuml
///    participant DM
///    participant MAC
///    participant F1
///    Note over MAC: UL CCCH subPDU decoded
///    MAC-->>DM: UL CCCH PDU Indication
///    Note over DM: Allocate UE index
///    Note over DM: Buffer UL CCCH Indication
///    DM->>F1: UE create
///    F1-->>DM: UE create completed
///    Note over DM: Create RLC SRB0 (Factory)
///    DM->>MAC: UE create
///    MAC-->>DM: UE create completed
///    Note over DM: Create UE in UE manager
///    DM->>MAC: Notify UL CCCH PDU
///    MAC->>F1: SRB0 - Notify UL CCCH PDU
///  \enduml
class ue_creation_procedure
{
public:
  ue_creation_procedure(du_ue_index_t                                ue_index,
                        const ul_ccch_indication_message&            ccch_ind_msg,
                        ue_manager_ctrl_configurator&                ue_mng_,
                        const du_manager_params::service_params&     du_services_,
                        const du_manager_params::mac_config_params&  mac_mng_,
                        const du_manager_params::rlc_config_params&  rlc_params_,
                        const du_manager_params::f1ap_config_params& f1ap_mng_,
                        du_ran_resource_manager&                     cell_res_alloc_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "UE Create"; }

private:
  /// Remove UE from DU Manager UE repository.
  void clear_ue();

  /// Setups DU manager resources used by DU UE being created.
  bool setup_du_ue_resources();

  /// Creates UE object in F1.
  void create_f1ap_ue();

  /// Creates SRB0 and SRB1 in RLC.
  void create_rlc_srbs();

  async_task<mac_ue_create_response_message> make_mac_ue_create_req();

  void connect_layer_bearers();

  ul_ccch_indication_message                   msg;
  ue_manager_ctrl_configurator&                ue_mng;
  const du_manager_params::service_params&     services;
  const du_manager_params::mac_config_params&  mac_mng;
  const du_manager_params::rlc_config_params&  rlc_cfg;
  const du_manager_params::f1ap_config_params& f1ap_mng;
  du_ran_resource_manager&                     du_res_alloc;
  srslog::basic_logger&                        logger;

  du_ue*                         ue_ctx = nullptr;
  mac_ue_create_response_message mac_resp{};
  f1ap_ue_creation_response      f1ap_resp{};
};

} // namespace srs_du
} // namespace srsran
