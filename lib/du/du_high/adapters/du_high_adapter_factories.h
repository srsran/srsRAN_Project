/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/du/du_high/du_test_mode_config.h"
#include "srsran/mac/mac.h"
#include "srsran/mac/mac_config.h"

namespace srsran {
namespace srs_du {

/// \brief Create a MAC instance for DU-high. In case the test mode is enabled, the MAC messages will be intercepted.
std::unique_ptr<mac_interface>
create_du_high_mac(const mac_config& mac_cfg, const srs_du::du_test_mode_config& test_cfg, unsigned nof_cells);

} // namespace srs_du
} // namespace srsran
