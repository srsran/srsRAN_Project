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

#include "srsran/ran/cyclic_prefix.h"
#include "srsran/ran/resource_allocation/rb_interval.h"

namespace srsran {

/// Generic parameters of a bandwidth part as defined in TS 38.211, clause 4.5 and TS 38.213, clause 12.
/// \remark See TS 38.331, Bandwidth-Part (BWP).
struct bwp_configuration {
  cyclic_prefix      cp;
  subcarrier_spacing scs;
  /// Common RBs where the BWP is located. CRB=0 overlaps with pointA.
  crb_interval crbs;

  bool operator==(const bwp_configuration& other) const
  {
    return cp == other.cp and scs == other.scs and crbs == other.crbs;
  }

  bool operator<(const bwp_configuration& other) const
  {
    return cp < other.cp or (cp == other.cp and scs < other.scs) or
           (cp == other.cp and scs == other.scs and crbs < other.crbs);
  }
};

} // namespace srsran
