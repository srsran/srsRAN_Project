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

#include "srsran/adt/span.h"
#include "srsran/ran/uci/uci_part2_size_description.h"

namespace srsran {
namespace fapi_adaptor {

/// \brief UCI Part2 sizes repository.
///
/// The repository stores arrays of Part2 sizes as a function of Part1 parameters.
class uci_part2_correspondence_repository
{
  /// Each entry contains a list of UCI Part2 sizes.
  std::vector<static_vector<uint16_t, uci_part2_size_description::max_size_table>> repo_map;

public:
  explicit uci_part2_correspondence_repository(
      std::vector<static_vector<uint16_t, uci_part2_size_description::max_size_table>>&& repo_map_) :
    repo_map(std::move(repo_map_))
  {
    srsran_assert(!repo_map.empty(), "Empty container");
  }

  /// \brief Returns a list of Part2 sizes related to Part1 parameters associated to the given index.
  ///
  /// \note Index value must be valid.
  span<const uint16_t> get_uci_part2_correspondence(unsigned index) const;
};

} // namespace fapi_adaptor
} // namespace srsran
