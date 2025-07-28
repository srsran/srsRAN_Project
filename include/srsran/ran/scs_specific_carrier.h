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

#include "srsran/ran/subcarrier_spacing.h"
#include <optional>

namespace srsran {

/// \brief It provides parameters determining the location and width of the actual carrier.
/// \remark See TS 38.331, "SCS-SpecificCarrier".
struct scs_specific_carrier {
  /// Offset between Point A (lowest subcarrier of common RB 0) and the lowest usable subcarrier in this carrier in
  /// number of PRBs. Values: (0..2199).
  unsigned           offset_to_carrier;
  subcarrier_spacing scs;
  /// Width of this carrier in number of PRBs. Values: (0..MAX_NOF_PRBS).
  unsigned carrier_bandwidth;
  /// Indicates the downlink Tx Direct Current location for the carrier. A value in the range 0..3299 indicates the
  /// subcarrier index within the carrier. The values in the value range 3301..4095 are reserved and ignored by the UE.
  /// If this field is absent, the UE assumes the default value of 3300 (i.e. "Outside the carrier").
  std::optional<unsigned> tx_direct_current_location;

  bool operator==(const scs_specific_carrier& rhs) const
  {
    return offset_to_carrier == rhs.offset_to_carrier && scs == rhs.scs && carrier_bandwidth == rhs.carrier_bandwidth &&
           tx_direct_current_location == rhs.tx_direct_current_location;
  }
  bool operator!=(const scs_specific_carrier& rhs) const { return !(rhs == *this); }
};

} // namespace srsran
