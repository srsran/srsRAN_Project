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

#include "srsran/adt/span.h"
#include "srsran/adt/static_vector.h"
#include <cstdint>

namespace srsran {

struct csi_report_configuration;

namespace fapi_adaptor {

/// UCI Part2 information fields that describe the UCI Part 1 correspondence to Part 2 sizes.
struct uci_part2_correspondence_information {
  /// Maximum number of parameters.
  static constexpr unsigned max_nof_part1_params = 4;

  /// Collects parameter attributes.
  struct part1_parameter {
    uint16_t offset;
    uint8_t  bitwidth;
  };

  /// Priority of the Part2 report.
  uint16_t priority;
  /// Part1 parameters.
  static_vector<part1_parameter, max_nof_part1_params> part1_params;
  /// Map index into the repository.
  uint16_t part2_map_index;
  /// Map scope.
  uint8_t part2_map_scope;
};

/// \brief UCI Part2 correspondence mapper.
///
/// Maps the given arguments to a a list of UCI Part2 correspondence entries.
class uci_part2_correspondence_mapper
{
public:
  static constexpr unsigned max_nof_part2_entries = 4;
  using correspondence_info_container = static_vector<uci_part2_correspondence_information, max_nof_part2_entries>;

  explicit uci_part2_correspondence_mapper(std::vector<correspondence_info_container>&& correspondence_map_) :
    correspondence_map(std::move(correspondence_map_))
  {
    srsran_assert(!correspondence_map.empty(), "Empty container");
  }

  /// Maps the given CSI report configuration into a list of correspondence entries.
  span<const uci_part2_correspondence_information> map(const csi_report_configuration& csi_report) const;

private:
  /// Each entry contains a list of UCI Part2 entries.
  std::vector<correspondence_info_container> correspondence_map;
};

} // namespace fapi_adaptor
} // namespace srsran
