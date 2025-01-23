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
