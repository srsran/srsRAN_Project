/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ue_creation_procedure.h"
#include "../adapters/mac_adapters.h"
#include "../adapters/rlc_adapters.h"

using namespace srsgnb;
using namespace srsgnb::srs_du;

void ue_creation_procedure::create_srb0()
{
  ue_ctx.bearers.emplace(LCID_SRB0);
  du_logical_channel_context& srb0 = ue_ctx.bearers[LCID_SRB0];
  srb0.lcid                        = LCID_SRB0;

  // Create RLC -> F1AP adapter.
  srb0.rlc_ul_sdu_notif = std::make_unique<rlc_rx_rrc_sdu_adapter>(*cfg.f1ap_rrc);

  // Create RLC CTRL adapter.
  srb0.rlc_ctrl_notif = std::make_unique<rlc_tx_control_notifier>();

  // Create RLC entity.
  rlc_entity_config rlc_cfg{};
  rlc_cfg.lcid     = LCID_SRB0;
  rlc_cfg.mode     = rlc_mode::TM;
  rlc_cfg.ue_index = ue_ctx.ue_index;
  rlc_cfg.upper_dn = srb0.rlc_ul_sdu_notif.get();
  rlc_cfg.upper_cn = srb0.rlc_ctrl_notif.get();
  srb0.rlc_bearer  = create_rlc_entity(rlc_cfg);

  // Create MAC -> RLC adapters.
  srb0.mac_rx_notifier = std::make_unique<mac_sdu_rx_adapter>(*srb0.rlc_bearer->get_rx_pdu_handler());
  srb0.mac_tx_notifier = std::make_unique<mac_sdu_tx_adapter>(*srb0.rlc_bearer->get_tx_pdu_transmitter());
}
