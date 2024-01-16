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

#include "srsran/adt/variant.h"

namespace srsran {

/// \brief Indicates the PRB bundle type and bundle size(s).
/// \remark See TS 38.214, clause 5.1.2.3.
struct prb_bundling {
  struct static_bundling {
    enum class bundling_size { n4, wideband };

    optional<bundling_size> sz;

    bool operator==(const static_bundling& rhs) const { return sz == rhs.sz; }
    bool operator!=(const static_bundling& rhs) const { return !(rhs == *this); }
  };

  struct dynamic_bundling {
    enum class bundling_size_set1 { n4, wideband, n2_wideband, n4_wideband };
    enum class bundling_size_set2 { n4, wideband };

    optional<bundling_size_set1> sz_set1;
    optional<bundling_size_set2> sz_set2;

    bool operator==(const dynamic_bundling& rhs) const { return sz_set1 == rhs.sz_set1 && sz_set2 == rhs.sz_set2; }
    bool operator!=(const dynamic_bundling& rhs) const { return !(rhs == *this); }
  };

  variant<static_bundling, dynamic_bundling> bundling;

  bool operator==(const prb_bundling& rhs) const { return bundling == rhs.bundling; }
  bool operator!=(const prb_bundling& rhs) const { return !(rhs == *this); }
};

} // namespace srsran
