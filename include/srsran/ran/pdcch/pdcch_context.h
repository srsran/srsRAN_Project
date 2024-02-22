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
#include "srsran/ran/pdcch/search_space.h"
#include "srsran/ran/rnti.h"

namespace srsran {

/// PDCCH transmission context.
class pdcch_context
{
public:
  /// Default constructor.
  pdcch_context() = default;

  explicit pdcch_context(search_space_id ss_id_, const char* dci_format_, optional<unsigned> harq_feedback_timing_) :
    ss_id(ss_id_), dci_format(dci_format_), harq_feedback_timing(harq_feedback_timing_)
  {
  }

  /// Comparison operators.
  bool operator==(const pdcch_context& other) const
  {
    return ss_id == other.ss_id && dci_format == other.dci_format && harq_feedback_timing == other.harq_feedback_timing;
  }
  bool operator!=(const pdcch_context& other) const { return !(*this == other); }

private:
  friend struct fmt::formatter<pdcch_context>;
  search_space_id    ss_id      = MAX_NOF_SEARCH_SPACES;
  const char*        dci_format = nullptr;
  optional<unsigned> harq_feedback_timing;
};

} // namespace srsran
