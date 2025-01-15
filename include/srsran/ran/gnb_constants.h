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

/// \brief Defines the maximum number of cells that can be instantiated per srsDU (implementation-defined).
/// \remark TS 38.473 defines an upper limit of maxCellingNBDU=512 for this value.
static constexpr uint16_t MAX_CELLS_PER_DU = 32;

} // namespace srsran
