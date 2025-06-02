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

#include "srsran/phy/support/precoding_configuration.h"
#include "srsran/phy/support/re_pattern.h"
#include "srsran/phy/upper/dmrs_mapping.h"
#include "srsran/ran/ptrs/ptrs.h"
#include "srsran/ran/rnti.h"
#include "srsran/ran/slot_point.h"

namespace srsran {

class resource_grid_writer;

/// Interface for the Phase Tracking Reference Signals (PT-RS) for PDSCH generator.
class ptrs_pdsch_generator
{
public:
  /// Required parameters to generate the signal as described in 3GPP TS38.211 Section 7.4.1.2.
  struct configuration {
    /// Slot context for sequence initialization.
    slot_point slot;
    /// RNTI associated with the DCI scheduling the transmission, parameter \f$n_{RNTI}\f$.
    rnti_t rnti;
    /// DM-RS config type (\e dmrsConfigType).
    dmrs_type dmrs_config_type;
    /// DM-RS for PDSCH reference point \e k in RBs, relative to Point A.
    unsigned reference_point_k_rb;
    /// PDSCH DMRS-Scrambling-ID (\e pdschDmrsScramblingId).
    unsigned scrambling_id;
    /// DM-RS for PDSCH sequence initialization (\f$n_{SCID}\f$).
    bool n_scid;
    /// Generated signal linear amplitude.
    float amplitude;
    /// DM-RS position mask. Indicates the OFDM symbols carrying DM-RS within the slot.
    symbol_slot_mask dmrs_symbols_mask;
    /// PDSCH frequency domain allocation as a CRB mask. The entries set to true are used for transmission.
    crb_bitmap rb_mask;
    /// PDSCH time domain allocation.
    interval<uint8_t> time_allocation;
    /// Frequency domain density.
    ptrs_frequency_density freq_density;
    /// Time domain density.
    ptrs_time_density time_density;
    /// Resource element offset.
    ptrs_re_offset re_offset;
    /// \brief Reserved RE pattern.
    ///
    /// Indicates the elements that cannot contain PT-RS as described in TS38.211 Section 7.4.1.2.2.
    re_pattern_list reserved;
    /// PDSCH precoding configuration.
    precoding_configuration precoding;
  };

  /// Default destructor.
  virtual ~ptrs_pdsch_generator() = default;

  /// \brief Generates and maps PT-RS for PDSCH.
  /// \param [out] grid Destination resource grid.
  /// \param [in]  config Required configuration to generate and map the signal.
  virtual void generate(resource_grid_writer& grid, const configuration& config) = 0;
};

} // namespace srsran
