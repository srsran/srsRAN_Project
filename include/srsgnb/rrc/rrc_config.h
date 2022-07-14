/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_RRC_CONFIG_H
#define SRSGNB_RRC_CONFIG_H

#include "rrc_ue_config.h"
#include "srsgnb/adt/optional.h"
#include "srsgnb/asn1/rrc_nr/rrc_nr.h"
#include <map>
#include <memory>

namespace srsgnb {

namespace srs_cu_cp {

struct srb_5g_cfg_t {
  asn1::rrc_nr::rlc_cfg_c rlc_cfg;
};

/// Configuration of a 5QI for RLC and PDCP.
struct five_qi_cfg_t {
  asn1::rrc_nr::pdcp_cfg_s pdcp_cfg;
  asn1::rrc_nr::rlc_cfg_c  rlc_cfg;
};

struct rrc_cfg_t {
  rrc_ue_cfg_t ue_default_cfg;

  optional<srb_5g_cfg_t>            srb2_cfg;    ///< Optional SRB2 config. SRB1 is configured by DU
  std::map<uint32_t, five_qi_cfg_t> five_qi_cfg; ///< Configuration for available 5QI.

  srslog::basic_logger& logger = srslog::fetch_basic_logger("RRC");
};

} // namespace srs_cu_cp

} // namespace srsgnb

#endif // SRSGNB_RRC_CONFIG_H
