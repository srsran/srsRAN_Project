/*
 *
 * Copyright 2021-2026 Software Radio Systems Limited
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

#include "srsran/fapi_adaptor/mac/p7/mac_fapi_p7_sector_fastpath_adaptor.h"
#include <memory>

namespace srsran {
namespace fapi_adaptor {

struct mac_fapi_p7_sector_fastpath_adaptor_dependencies;
struct mac_fapi_p7_sector_fastpath_adaptor_config;

/// Creates a MAC-FAPI P7 sector fastpath adaptor.
std::unique_ptr<mac_fapi_p7_sector_fastpath_adaptor>
create_mac_fapi_p7_sector_fastpath_adaptor(const mac_fapi_p7_sector_fastpath_adaptor_config& config,
                                           mac_fapi_p7_sector_fastpath_adaptor_dependencies  dependencies);

} // namespace fapi_adaptor
} // namespace srsran
