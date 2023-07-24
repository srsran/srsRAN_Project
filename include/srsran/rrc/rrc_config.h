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

#include "rrc_ue_config.h"
#include "srsran/adt/optional.h"
#include "srsran/asn1/rrc_nr/rrc_nr.h"
#include <map>
#include <memory>

namespace srsran {

namespace srs_cu_cp {

/// RLC and PDCP configuration of a radio bearer.
struct rb_cfg_t {
  asn1::rrc_nr::pdcp_cfg_s pdcp_cfg;
  asn1::rrc_nr::rlc_cfg_c  rlc_cfg;
};

struct rrc_cfg_t {
  asn1::rrc_nr::pdcp_cfg_s              srb1_pdcp_cfg; ///< PDCP configuration for SRB1.
  optional<rb_cfg_t>                    srb2_cfg;      ///< Optional SRB2 config. SRB1 is configured by DU
  std::map<five_qi_t, cu_cp_qos_config> drb_config;    ///< Configuration for available 5QI.
  bool                                  force_reestablishment_fallback = false; ///< Force re-establishment fallback.
};

} // namespace srs_cu_cp

} // namespace srsran
