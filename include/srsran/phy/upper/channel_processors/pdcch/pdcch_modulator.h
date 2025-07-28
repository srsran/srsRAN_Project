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

#include "srsran/adt/span.h"
#include "srsran/phy/support/precoding_configuration.h"
#include "srsran/ran/resource_allocation/rb_bitmap.h"

namespace srsran {

class resource_grid_writer;

/// \brief Describes a PDCCH modulator interface.
///
/// \remark Defined in TS38.211 Section 7.3.2 Physical downlink control channel (PDCCH).
/// \note The configuration assumes the CCE-to-REG and REG-to-PRB mapping is included in \c rb_mask.
class pdcch_modulator
{
public:
  /// Describes the necessary parameters to modulate a PDCCH transmission.
  struct config_t {
    /// Indicates the RBs used for the PDCCH transmission.
    crb_bitmap rb_mask;
    /// CORESET start symbol index.
    unsigned start_symbol_index;
    /// CORESET duration in symbols.
    unsigned duration;
    /// Higher layer parameter PDCCH-DMRS-ScramblingID if it is given, otherwise the physical cell identifier.
    unsigned n_id;
    /// Parameter \f$n_{RNTI}\f$ used for PDCCH data scrambling according to TS38.211 Section 7.3.2.3.
    unsigned n_rnti;
    /// Scaling factor to apply to the resource elements according to PDCCH power allocation in TS38.213.
    float scaling;
    /// Precoding information for the PDCCH transmission.
    precoding_configuration precoding;
  };

  /// Default destructor.
  virtual ~pdcch_modulator() = default;

  /// \brief Modulates a PDCCH codeword according to TS38.211 Section 7.3.2.
  /// \param[out] grid   Resource grid writer interface.
  /// \param[in] data    PDCCH codeword to modulate.
  /// \param[in] config  Necessary parameters for modulating PDCCH.
  /// \remark The codeword length shall be consistent with the resource mapping.
  /// \remark An assertion is triggered if the number of precoding layers is not one.
  virtual void modulate(resource_grid_writer& grid, span<const uint8_t> data, const config_t& config) = 0;
};

} // namespace srsran
