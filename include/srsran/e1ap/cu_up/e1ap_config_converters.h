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
  cfg.tx.rb_type = pdcp_rb_type::drb;
  cfg.rx.rb_type = pdcp_rb_type::drb;

  // RLC mode
  if (e1ap_cfg.rlc_mod == rlc_mode::um_bidir) {
    cfg.tx.rlc_mode = pdcp_rlc_mode::um;
    cfg.rx.rlc_mode = pdcp_rlc_mode::um;
  } else if (e1ap_cfg.rlc_mod == rlc_mode::am) {
    cfg.tx.rlc_mode = pdcp_rlc_mode::am;
    cfg.rx.rlc_mode = pdcp_rlc_mode::am;
  } else {
    report_fatal_error("Unsupported RLC mode for DRB. RLC mode={}", e1ap_cfg.rlc_mod);
  }

  // SN size
  cfg.tx.sn_size = e1ap_cfg.pdcp_sn_size_dl;
  cfg.rx.sn_size = e1ap_cfg.pdcp_sn_size_ul;

  // Direction
  cfg.tx.direction = pdcp_security_direction::downlink;
  cfg.rx.direction = pdcp_security_direction::uplink;

  // Integrity protection required
  cfg.tx.integrity_protection_required = false; // FIXME check this
  cfg.rx.integrity_protection_required = false;

  // Ciphering required
  cfg.tx.ciphering_required = false; // FIXME check this
  cfg.rx.ciphering_required = false;

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
