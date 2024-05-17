/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/rlc/rlc_config.h"

namespace srsran {

/// \brief SRB0 default configuration (only implementation-specific parameters)
inline rlc_config make_default_srb0_rlc_config()
{
  rlc_config cfg       = {};
  cfg.mode             = rlc_mode::tm;
  cfg.tm.tx.queue_size = 8;
  cfg.metrics_period   = std::chrono::milliseconds(0); // disable metrics reporting for SRBs
  return cfg;
}

/// \brief SRB1, SRB2 and SRB3 default configuration as per TS 38.331, 9.2.1.
inline rlc_config make_default_srb_rlc_config()
{
  rlc_config cfg              = {};
  cfg.mode                    = rlc_mode::am;
  cfg.am.tx.sn_field_length   = rlc_am_sn_size::size12bits;
  cfg.am.tx.pdcp_sn_len       = pdcp_sn_size::size12bits;
  cfg.am.tx.t_poll_retx       = 45;
  cfg.am.tx.poll_pdu          = -1;
  cfg.am.tx.poll_byte         = -1;
  cfg.am.tx.max_retx_thresh   = 8;
  cfg.am.tx.queue_size        = 32;
  cfg.am.rx.sn_field_length   = rlc_am_sn_size::size12bits;
  cfg.am.rx.t_reassembly      = 35;
  cfg.am.rx.t_status_prohibit = 0;
  cfg.am.rx.max_sn_per_status = {};
  cfg.metrics_period          = std::chrono::milliseconds(0); // disable metrics reporting for SRBs
  return cfg;
}

} // namespace srsran
