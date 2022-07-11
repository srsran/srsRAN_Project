/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_LIB_DU_MANAGER_PROCEDURES_UE_CREATION_PROCEDURE_H
#define SRSGNB_LIB_DU_MANAGER_PROCEDURES_UE_CREATION_PROCEDURE_H

#include "../../ran/gnb_format.h"
#include "../converters/mac_cell_configuration_helpers.h"
#include "../du_manager_config.h"
#include "../du_manager_interfaces.h"
#include "../du_ue_context.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/rlc/rlc.h"
#include "srsgnb/rlc/rlc_factory.h"
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
  ue_creation_procedure(du_ue_index_t                     ue_index,
                        const ul_ccch_indication_message& ccch_ind_msg,
                        const du_manager_config_t&        cfg_,
                        ue_manager_ctrl_configurer&       ue_mng_);

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

  /// Creates SRB0 in RLC and connects it to MAC and F1.
  void create_srb0();

  async_task<mac_ue_create_response_message> make_mac_ue_create_req();

  async_task<f1ap_ue_create_response> make_f1_ue_create_req();

  ul_ccch_indication_message  msg;
  const du_manager_config_t&  cfg;
  srslog::basic_logger&       logger;
  ue_manager_ctrl_configurer& ue_mng;

  du_ue_context                  ue_ctx{};
  mac_ue_create_response_message mac_resp{};
  f1ap_ue_create_response        f1_resp{};
};

} // namespace srs_du
} // namespace srsgnb

#endif // SRSGNB_LIB_DU_MANAGER_PROCEDURES_UE_CREATION_PROCEDURE_H
