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
