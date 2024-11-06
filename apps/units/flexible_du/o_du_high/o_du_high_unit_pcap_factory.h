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

#include "du_high/pcap_factory.h"
#include "o_du_high_unit_config.h"

namespace srsran {

/// Creates the PCAPs of the DU.
inline flexible_du_pcaps create_o_du_pcaps(const o_du_high_unit_config& unit_cfg, worker_manager& workers)
{
  return create_du_pcaps(unit_cfg.du_high_cfg.config.pcaps, workers);
}

} // namespace srsran
