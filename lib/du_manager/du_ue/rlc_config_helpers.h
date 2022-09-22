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

#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/lcid.h"
#include "srsgnb/rlc/rlc_config.h"

namespace srsgnb {
namespace srs_du {

struct rlc_bearer_config {
  lcid_t     lcid;
  drb_id_t   drb_id = drb_id_t::invalid;
  rlc_config rlc_cfg;

  bool operator==(const rlc_bearer_config& rhs) const
  {
    // TODO: Remaining fields
    return lcid == rhs.lcid and drb_id == rhs.drb_id and rlc_cfg.mode == rhs.rlc_cfg.mode;
  }
};

inline rlc_bearer_config make_default_srb(lcid_t lcid)
{
  rlc_bearer_config cfg;
  cfg.lcid = lcid;
  if (lcid == LCID_SRB0) {
    cfg.rlc_cfg.mode = rlc_mode::tm;
    return cfg;
  }
  cfg.rlc_cfg.mode                    = rlc_mode::am;
  cfg.rlc_cfg.am.tx.sn_field_length   = rlc_am_sn_size::size12bits;
  cfg.rlc_cfg.am.tx.t_poll_retx       = 45;
  cfg.rlc_cfg.am.tx.poll_pdu          = -1;
  cfg.rlc_cfg.am.tx.poll_byte         = -1;
  cfg.rlc_cfg.am.tx.max_retx_thresh   = 8;
  cfg.rlc_cfg.am.rx.sn_field_length   = rlc_am_sn_size::size12bits;
  cfg.rlc_cfg.am.rx.t_reassembly      = 35;
  cfg.rlc_cfg.am.rx.t_status_prohibit = 0;
  return cfg;
}

} // namespace srs_du
} // namespace srsgnb