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

#include <cstdint>

namespace srsran {
namespace fapi {

/// Common enumerations for DL_TTI.request and UL_TTI.request.
enum class dmrs_cfg_type : uint8_t { type_1, type_2 };
enum class low_papr_dmrs_type : uint8_t { independent_cdm_group, dependent_cdm_group };
enum class resource_allocation_type : uint8_t { type_0, type_1 };
enum class vrb_to_prb_mapping_type : uint8_t { non_interleaved, interleaved_rb_size2, interleaved_rb_size4 };

} // namespace fapi
} // namespace srsran
