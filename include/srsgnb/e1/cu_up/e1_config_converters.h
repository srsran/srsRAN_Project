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

#include "srsgnb/asn1/e1ap/e1ap.h"
#include "srsgnb/pdcp/pdcp_config.h"

namespace srsgnb {

inline pdcp_config make_pdcp_drb_config(const asn1::e1ap::pdcp_cfg_s& e1ap_cfg)
{
  pdcp_config cfg = {};

  //////////////////////////////
  // Fill TX/RX common config //
  //////////////////////////////
  // RB type
  cfg.tx.rb_type = pdcp_rb_type::drb;
  cfg.rx.rb_type = pdcp_rb_type::drb;

  // RLC mode
  if (e1ap_cfg.rlc_mode == asn1::e1ap::rlc_mode_opts::options::rlc_um_bidirectional) {
    cfg.tx.rlc_mode = pdcp_rlc_mode::um;
    cfg.rx.rlc_mode = pdcp_rlc_mode::um;
  } else if (e1ap_cfg.rlc_mode == asn1::e1ap::rlc_mode_opts::options::rlc_am) {
    cfg.tx.rlc_mode = pdcp_rlc_mode::am;
    cfg.rx.rlc_mode = pdcp_rlc_mode::am;
  } else {
    srsgnb_assertion_failure("Unsupported RLC mode for DRB. RLC mode={}", e1ap_cfg.rlc_mode);
  }

  // SN size
  if (e1ap_cfg.pdcp_sn_size_dl == asn1::e1ap::pdcp_sn_size_opts::options::s_neg12) {
    cfg.tx.sn_size = pdcp_sn_size::size12bits;
  } else if (e1ap_cfg.pdcp_sn_size_dl == asn1::e1ap::pdcp_sn_size_opts::options::s_neg18) {
    cfg.tx.sn_size = pdcp_sn_size::size18bits;
  } else {
    srsgnb_assertion_failure("Unsupported PDCP SN size for DRB DL. PDCP SN size={}", e1ap_cfg.pdcp_sn_size_dl);
  }
  if (e1ap_cfg.pdcp_sn_size_ul == asn1::e1ap::pdcp_sn_size_opts::options::s_neg12) {
    cfg.rx.sn_size = pdcp_sn_size::size12bits;
  } else if (e1ap_cfg.pdcp_sn_size_ul == asn1::e1ap::pdcp_sn_size_opts::options::s_neg18) {
    cfg.rx.sn_size = pdcp_sn_size::size18bits;
  } else {
    srsgnb_assertion_failure("Unsupported PDCP SN size for DRB UL. PDCP SN size={}", e1ap_cfg.pdcp_sn_size_ul);
  }

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
  if (not e1ap_cfg.discard_timer_present) {
    cfg.tx.discard_timer = pdcp_discard_timer::not_configured;
  } else {
    switch (e1ap_cfg.discard_timer) {
      case asn1::e1ap::discard_timer_opts::ms10:
        cfg.tx.discard_timer = pdcp_discard_timer::ms10;
        break;
      case asn1::e1ap::discard_timer_opts::ms20:
        cfg.tx.discard_timer = pdcp_discard_timer::ms20;
        break;
      case asn1::e1ap::discard_timer_opts::ms30:
        cfg.tx.discard_timer = pdcp_discard_timer::ms30;
        break;
      case asn1::e1ap::discard_timer_opts::ms40:
        cfg.tx.discard_timer = pdcp_discard_timer::ms40;
        break;
      case asn1::e1ap::discard_timer_opts::ms50:
        cfg.tx.discard_timer = pdcp_discard_timer::ms50;
        break;
      case asn1::e1ap::discard_timer_opts::ms60:
        cfg.tx.discard_timer = pdcp_discard_timer::ms60;
        break;
      case asn1::e1ap::discard_timer_opts::ms75:
        cfg.tx.discard_timer = pdcp_discard_timer::ms75;
        break;
      case asn1::e1ap::discard_timer_opts::ms100:
        cfg.tx.discard_timer = pdcp_discard_timer::ms100;
        break;
      case asn1::e1ap::discard_timer_opts::ms150:
        cfg.tx.discard_timer = pdcp_discard_timer::ms150;
        break;
      case asn1::e1ap::discard_timer_opts::ms200:
        cfg.tx.discard_timer = pdcp_discard_timer::ms200;
        break;
      case asn1::e1ap::discard_timer_opts::ms250:
        cfg.tx.discard_timer = pdcp_discard_timer::ms250;
        break;
      case asn1::e1ap::discard_timer_opts::ms300:
        cfg.tx.discard_timer = pdcp_discard_timer::ms300;
        break;
      case asn1::e1ap::discard_timer_opts::ms500:
        cfg.tx.discard_timer = pdcp_discard_timer::ms500;
        break;
      case asn1::e1ap::discard_timer_opts::ms750:
        cfg.tx.discard_timer = pdcp_discard_timer::ms750;
        break;
      case asn1::e1ap::discard_timer_opts::ms1500:
        cfg.tx.discard_timer = pdcp_discard_timer::ms1500;
        break;
      case asn1::e1ap::discard_timer_opts::infinity:
        cfg.tx.discard_timer = pdcp_discard_timer::infinity;
        break;
      default:
        srsgnb_assertion_failure("Unsupported PDCP discard tiemr. PDCP discard timer={}",
                                 e1ap_cfg.discard_timer.to_string());
    }

    // Status report required
    // FIXME update ASN1 to include status report required, as included in 38.463, v16.3
    cfg.tx.status_report_required = false;

    // RX configuration
    // Out-of-order delivery
    if (e1ap_cfg.out_of_order_delivery_present) {
      cfg.rx.out_of_order_delivery = true;
    }

    // t-Reordering
    switch (e1ap_cfg.t_reordering_timer.t_reordering) {
      case asn1::e1ap::t_reordering_e::ms0:
        cfg.rx.t_reordering = pdcp_t_reordering::ms0;
        break;
      case asn1::e1ap::t_reordering_e::ms1:
        cfg.rx.t_reordering = pdcp_t_reordering::ms1;
        break;
      case asn1::e1ap::t_reordering_e::ms2:
        cfg.rx.t_reordering = pdcp_t_reordering::ms2;
        break;
      case asn1::e1ap::t_reordering_e::ms4:
        cfg.rx.t_reordering = pdcp_t_reordering::ms4;
        break;
      case asn1::e1ap::t_reordering_e::ms5:
        cfg.rx.t_reordering = pdcp_t_reordering::ms5;
        break;
      case asn1::e1ap::t_reordering_e::ms8:
        cfg.rx.t_reordering = pdcp_t_reordering::ms8;
        break;
      case asn1::e1ap::t_reordering_e::ms10:
        cfg.rx.t_reordering = pdcp_t_reordering::ms10;
        break;
      case asn1::e1ap::t_reordering_e::ms15:
        cfg.rx.t_reordering = pdcp_t_reordering::ms15;
        break;
      case asn1::e1ap::t_reordering_e::ms20:
        cfg.rx.t_reordering = pdcp_t_reordering::ms20;
        break;
      case asn1::e1ap::t_reordering_e::ms30:
        cfg.rx.t_reordering = pdcp_t_reordering::ms30;
        break;
      case asn1::e1ap::t_reordering_e::ms40:
        cfg.rx.t_reordering = pdcp_t_reordering::ms40;
        break;
      case asn1::e1ap::t_reordering_e::ms50:
        cfg.rx.t_reordering = pdcp_t_reordering::ms50;
        break;
      case asn1::e1ap::t_reordering_e::ms60:
        cfg.rx.t_reordering = pdcp_t_reordering::ms60;
        break;
      case asn1::e1ap::t_reordering_e::ms80:
        cfg.rx.t_reordering = pdcp_t_reordering::ms80;
        break;
      case asn1::e1ap::t_reordering_e::ms100:
        cfg.rx.t_reordering = pdcp_t_reordering::ms100;
        break;
      case asn1::e1ap::t_reordering_e::ms120:
        cfg.rx.t_reordering = pdcp_t_reordering::ms120;
        break;
      case asn1::e1ap::t_reordering_e::ms140:
        cfg.rx.t_reordering = pdcp_t_reordering::ms140;
        break;
      case asn1::e1ap::t_reordering_e::ms160:
        cfg.rx.t_reordering = pdcp_t_reordering::ms160;
        break;
      case asn1::e1ap::t_reordering_e::ms180:
        cfg.rx.t_reordering = pdcp_t_reordering::ms180;
        break;
      case asn1::e1ap::t_reordering_e::ms200:
        cfg.rx.t_reordering = pdcp_t_reordering::ms200;
        break;
      case asn1::e1ap::t_reordering_e::ms220:
        cfg.rx.t_reordering = pdcp_t_reordering::ms220;
        break;
      case asn1::e1ap::t_reordering_e::ms240:
        cfg.rx.t_reordering = pdcp_t_reordering::ms240;
        break;
      case asn1::e1ap::t_reordering_e::ms260:
        cfg.rx.t_reordering = pdcp_t_reordering::ms260;
        break;
      case asn1::e1ap::t_reordering_e::ms280:
        cfg.rx.t_reordering = pdcp_t_reordering::ms280;
        break;
      case asn1::e1ap::t_reordering_e::ms300:
        cfg.rx.t_reordering = pdcp_t_reordering::ms300;
        break;
      case asn1::e1ap::t_reordering_e::ms500:
        cfg.rx.t_reordering = pdcp_t_reordering::ms500;
        break;
      case asn1::e1ap::t_reordering_e::ms750:
        cfg.rx.t_reordering = pdcp_t_reordering::ms750;
        break;
      case asn1::e1ap::t_reordering_e::ms1000:
        cfg.rx.t_reordering = pdcp_t_reordering::ms1000;
        break;
      case asn1::e1ap::t_reordering_e::ms1250:
        cfg.rx.t_reordering = pdcp_t_reordering::ms1250;
        break;
      case asn1::e1ap::t_reordering_e::ms1500:
        cfg.rx.t_reordering = pdcp_t_reordering::ms1500;
        break;
      case asn1::e1ap::t_reordering_e::ms1750:
        cfg.rx.t_reordering = pdcp_t_reordering::ms1750;
        break;
      case asn1::e1ap::t_reordering_e::ms2000:
        cfg.rx.t_reordering = pdcp_t_reordering::ms2000;
        break;
      case asn1::e1ap::t_reordering_e::ms2250:
        cfg.rx.t_reordering = pdcp_t_reordering::ms2250;
        break;
      case asn1::e1ap::t_reordering_e::ms2500:
        cfg.rx.t_reordering = pdcp_t_reordering::ms2500;
        break;
      case asn1::e1ap::t_reordering_e::ms2750:
        cfg.rx.t_reordering = pdcp_t_reordering::ms2750;
        break;
      case asn1::e1ap::t_reordering_e::ms3000:
        cfg.rx.t_reordering = pdcp_t_reordering::ms3000;
        break;
      default:
        srsgnb_assertion_failure("Unsupported PDCP t-reordering timer. PDCP t-reordering timer={}",
                                 e1ap_cfg.t_reordering_timer.t_reordering.to_string());
    }
  }
  return cfg;
}
} // namespace srsgnb
