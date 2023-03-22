/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/e1ap/common/e1ap_types.h"
#include "srsran/pdcp/pdcp_config.h"

namespace srsran {

inline pdcp_config make_pdcp_drb_config(const e1ap_pdcp_config& e1ap_cfg)
{
  pdcp_config cfg = {};

  //////////////////////////////
  // Fill TX/RX common config //
  //////////////////////////////
  // RB type
  cfg.rb_type = pdcp_rb_type::drb;

  // RLC mode
  cfg.rlc_mode = e1ap_cfg.rlc_mod;

  // Integrity protection required
  cfg.integrity_protection_required = false; // FIXME check this

  // Ciphering required
  cfg.ciphering_required = false; // FIXME check this

  // SN size
  cfg.tx.sn_size = e1ap_cfg.pdcp_sn_size_dl; // TODO check UL or DL from the UE or gNB perspective.
  cfg.rx.sn_size = e1ap_cfg.pdcp_sn_size_ul;

  // Direction
  cfg.tx.direction = pdcp_security_direction::downlink;
  cfg.rx.direction = pdcp_security_direction::uplink;

  // TX config
  // Discard timer
  if (not e1ap_cfg.discard_timer.has_value()) {
    cfg.tx.discard_timer = pdcp_discard_timer::not_configured;
  } else {
    cfg.tx.discard_timer = e1ap_cfg.discard_timer.value();
  }

  // Status report required
  // FIXME update ASN1 to include status report required, as included in 38.463, v16.3
  cfg.tx.status_report_required = false;

  // RX configuration
  // Out-of-order delivery
  if (e1ap_cfg.out_of_order_delivery.has_value()) {
    cfg.rx.out_of_order_delivery = true;
  }

  // t-Reordering
  if (e1ap_cfg.t_reordering_timer.has_value()) {
    cfg.rx.t_reordering = e1ap_cfg.t_reordering_timer.value();
  } else {
    cfg.rx.t_reordering = pdcp_t_reordering::infinity;
  }

  return cfg;
}
} // namespace srsran
