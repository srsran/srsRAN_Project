/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../../ran/gnb_format.h"
#include "../converters/mac_cell_configuration_helpers.h"
#include "../du_manager_config.h"
#include "../du_ue/du_ue.h"
#include "../du_ue/ue_manager_ctrl_configurator.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/rlc/rlc_factory.h"
#include "srsgnb/rlc/rlc_rx.h"
#include "srsgnb/rlc/rlc_tx.h"
#include "srsgnb/support/async/async_task.h"

namespace srsgnb {
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
  void clear_ue()
  {
    if (f1_resp.result) {
      // TODO: F1 UE removal.
    }
    // TODO: MAC RNTI needs to be cleared.
  }

  /// Setups DU manager resources used by DU UE being created.
  bool setup_du_ue_resources();

  /// Creates UE object in F1.
  void create_f1_ue();

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

  std::unique_ptr<du_ue>         ue_ctx;
  mac_ue_create_response_message mac_resp{};
  f1ap_ue_creation_response      f1_resp{};
};

} // namespace srs_du
} // namespace srsgnb
