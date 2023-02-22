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

#include "srsran/asn1/rrc_nr/rrc_nr.h"
#include "srsran/rrc/drb_manager.h"
#include "srsran/srslog/srslog.h"

namespace srsran {

namespace srs_cu_cp {
struct rrc_ue_cfg_t {
  srslog::basic_logger&              logger = srslog::fetch_basic_logger("RRC");
  asn1::rrc_nr::pdcp_cfg_s           srb1_pdcp_cfg; ///< PDCP configuration for SRB1.
  srsran::srs_cu_cp::drb_manager_cfg drb_cfg; ///< DRB manager configuration holds the bearer configs for all FiveQIs.
};

} // namespace srs_cu_cp

} // namespace srsran
