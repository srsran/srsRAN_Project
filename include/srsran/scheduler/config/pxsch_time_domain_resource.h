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

#include "srsran/ran/resource_allocation/ofdm_symbol_range.h"

namespace srsran {

/// \brief Physical shared channels Mapping Type.
/// \remark see TS38.214 Section 5.3 for PDSCH and TS38.214 Section 6.4 for PUSCH.
enum class sch_mapping_type : uint8_t {
  /// TypeA time allocation, it can start only at symbol 2 or 3 within a slot.
  typeA,
  /// TypeB time allocation.
  typeB
};

/// \brief Collects the PDSCH default time-domain allocation parameters.
///
/// The parameters are provided by TS38.214 Tables 5.1.2.1.1-2, 5.1.2.1.1-3, 5.1.2.1.1-4 and 5.1.2.1.1-5, and in
/// TS 38.331.
/// A configuration is invalid if the duration is zero.
struct pdsch_time_domain_resource_allocation {
  /// PDCCH to PDSCH delay in slots, parameter \f$K_0\f$. Values: (0..32).
  uint8_t k0;
  /// PDSCH mapping.
  sch_mapping_type map_type;
  /// \brief Symbols used within the slot. Parameters \f$S\f$ and \f$L\f$. Values for \f$S\f$ are (0..10) and for
  /// \f$L\f$, (2..12).
  ofdm_symbol_range symbols;

  bool operator==(const pdsch_time_domain_resource_allocation& rhs) const
  {
    return k0 == rhs.k0 && map_type == rhs.map_type && symbols == rhs.symbols;
  }
  bool operator!=(const pdsch_time_domain_resource_allocation& rhs) const { return !(rhs == *this); }
};

/// \brief Collects the PUSCH default time-domain allocation parameters.
///
/// The parameters are provided by TS38.214 Tables 5.1.2.1.1-2, 5.1.2.1.1-3, 5.1.2.1.1-4 and 5.1.2.1.1-5, and in
/// TS 38.331.
///
/// A configuration is invalid if the duration is zero.
struct pusch_time_domain_resource_allocation {
  /// PDCCH to PUSCH delay in slots, parameter \f$K_2\f$. Values: (0..32).
  unsigned k2;
  /// PUSCH mapping.
  sch_mapping_type map_type;
  /// \brief Symbols used within the slot. Parameters \f$S\f$ and \f$L\f$. Values for \f$S\f$ are (0..10) and for
  /// \f$L\f$, (2..12).
  ofdm_symbol_range symbols;

  bool operator==(const pusch_time_domain_resource_allocation& rhs) const
  {
    return k2 == rhs.k2 && map_type == rhs.map_type && symbols == rhs.symbols;
  }
  bool operator!=(const pusch_time_domain_resource_allocation& rhs) const { return !(rhs == *this); }
};

} // namespace srsran
