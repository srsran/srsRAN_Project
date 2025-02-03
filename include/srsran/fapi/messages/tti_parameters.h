/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
