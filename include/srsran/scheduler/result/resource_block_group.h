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

#include "srsran/adt/bounded_bitset.h"
#include "srsran/ran/resource_allocation/rb_interval.h"

namespace srsran {

/// Maximum number of resource block groups per BWP.
/// \remark See TS 38.214, Sections 5.1.2.2.1 and 6.1.2.2.1.
const size_t MAX_NOF_RBGS = 18;

/// \brief RBG size types for PDSCH.
/// \remark See TS 38.331, "rbg-Size".
enum class rbg_size { config1, config2 };

/// Nominal RBG-size (P value).
/// \remark See TS 38.214, Sections 5.1.2.2 and 6.1.2.2.
enum class nominal_rbg_size : unsigned { P2 = 2, P4 = 4, P8 = 8, P16 = 16 };

inline unsigned to_nominal_rbg_size_value(nominal_rbg_size sz)
{
  return static_cast<unsigned>(sz);
}

/// Bitset of RBGs with size up to 18.
using rbg_bitmap = bounded_bitset<MAX_NOF_RBGS, true>;

/// Calculates Nominal RBG size (P) based on the BWP number of PRBs and configuration type.
/// \remark See TS 38.214, Table 5.1.2.2.1-1 and 6.1.2.2.1-1.
nominal_rbg_size get_nominal_rbg_size(unsigned bwp_nof_prb, bool is_config_1);

/// Calculates the total number of RBGs for a bandwidth part.
/// \remark See TS 38.214, Sections 5.1.2.2.1 and 6.1.2.2.1.
unsigned get_nof_rbgs(crb_interval bwp_rb_dims, nominal_rbg_size P);
unsigned get_nof_rbgs(crb_interval bwp_rb_dims, bool config1_or_2);

/// Calculates size of indexed RBG in number of RBs.
/// \remark See TS 38.214, Sections 5.1.2.2.1 and 6.1.2.2.1.
uint32_t get_rbg_size(crb_interval bwp_rbs, nominal_rbg_size P, uint32_t rbg_idx);

} // namespace srsran
