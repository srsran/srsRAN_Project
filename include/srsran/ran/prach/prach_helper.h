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

#include "rach_config_common.h"
#include "srsran/adt/expected.h"
#include "srsran/adt/interval.h"
#include "srsran/ran/cyclic_prefix.h"

namespace srsran {

struct tdd_ul_dl_config_common;

namespace prach_helper {

/// \brief Finds whether a PRACH config index fits in the provided TDD pattern.
/// \return In case of failure, returns the range of PRACH slots that did not fit in the TDD pattern. If the PRACH
/// configuration is invalid, an empty interval of slots is returned.
error_type<interval<uint8_t>> prach_fits_in_tdd_pattern(subcarrier_spacing             pusch_scs,
                                                        cyclic_prefix                  cp,
                                                        uint8_t                        prach_cfg_idx,
                                                        const tdd_ul_dl_config_common& tdd_cfg);

/// \brief Finds a PRACH configuration index that ensures that PRACH falls in an TDD UL slot.
optional<uint8_t>
find_valid_prach_config_index(subcarrier_spacing pusch_scs, cyclic_prefix cp, const tdd_ul_dl_config_common& tdd_cfg);

} // namespace prach_helper
} // namespace srsran