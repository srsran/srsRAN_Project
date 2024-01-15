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

#include "../du_ue/du_ue.h"
#include "../du_ue/du_ue_manager_repository.h"
#include "procedure_logger.h"
#include "srsran/du_manager/du_manager_params.h"
#include "srsran/mac/config/mac_config_helpers.h"
#include "srsran/mac/mac.h"
#include "srsran/support/async/async_task.h"

namespace srsran {
namespace srs_du {

struct du_ue_creation_request {
  du_ue_index_t   ue_index;
  du_cell_index_t pcell_index;
  rnti_t          tc_rnti;
  byte_buffer     ul_ccch_msg;
};

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
  ue_creation_procedure(const du_ue_creation_request& req_,
                        du_ue_manager_repository&     ue_mng_,
                        const du_manager_params&      du_params_,
                        du_ran_resource_manager&      cell_res_alloc_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "UE Create"; }

private:
  /// Creates a UE object in the DU UE manager.
  du_ue* create_du_ue_context();

  /// Remove UE from DU Manager UE repository.
  async_task<void> clear_ue();

  /// Setups DU manager resources used by DU UE being created.
  bool setup_du_ue_resources();

  /// Creates UE object in F1.
  f1ap_ue_creation_response create_f1ap_ue();

  /// Creates SRB0 and SRB1 in RLC.
  void create_rlc_srbs();

  async_task<mac_ue_create_response> create_mac_ue();

  void connect_layer_bearers();

  const du_ue_creation_request req;
  du_ue_manager_repository&    ue_mng;
  const du_manager_params&     du_params;
  du_ran_resource_manager&     du_res_alloc;
  ue_procedure_logger          proc_logger;

  du_ue*                    ue_ctx = nullptr;
  mac_ue_create_response    mac_resp{};
  f1ap_ue_creation_response f1ap_resp{};
};

} // namespace srs_du
} // namespace srsran
