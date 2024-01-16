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
#include "srsran/ran/uci/uci_part2_size_description.h"
#include "srsran/support/units.h"

namespace srsran {

/// Collects the parameters that describe the CSI report size transmitted in PUSCH.
struct csi_report_pusch_size {
  /// \brief CSI Part 1 number of bits.
  ///
  /// It is the total number of CSI Part 1 reports described in TS38.212 Table 6.3.2.1.2-6.
  units::bits part1_size;

  /// \brief CSI Part 1 correspondence to CSI Part 2 size.
  ///
  /// It provides the necessary information for calculating the CSI Part 2 reports payload size from the CSI Part 1
  /// decoded payload. The CSI Part 2 payload is described in TS38.212 Table 6.3.2.1.2-7.
  uci_part2_size_description part2_correspondence;

  /// \brief CSI Part 2 minimum payload size.
  ///
  /// Minimum number of CSI Part 2 payload bits given \c part2_correspondence. It is calculated by
  /// adding the minimum CSI Part 2 report sizes for all entries in \c part2_correpondence.
  units::bits part2_min_size;

  /// \brief CSI Part 2 maximum payload size.
  ///
  /// Maximum number of CSI Part 2 payload bits given \c part2_correspondence. It is calculated by
  /// adding the maximum CSI Part 2 report sizes for all entries in \c part2_correpondence.
  units::bits part2_max_size;
};

} // namespace srsran