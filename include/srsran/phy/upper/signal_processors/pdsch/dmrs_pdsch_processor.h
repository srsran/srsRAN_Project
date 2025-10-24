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
#include "srsran/phy/upper/dmrs_mapping.h"
#include "srsran/ran/slot_point.h"

namespace srsran {

class resource_grid_writer;

/// Describes a DMRS for PDSCH processor interface.
class dmrs_pdsch_processor
{
public:
  /// Describes the required parameters to generate the signal as described in 3GPP TS38.211 Section 7.4.1.1.
  struct config_t {
    /// Slot context for sequence initialization.
    slot_point slot;
    /// Reference point for PDSCH DM-RS \e k in RBs.
    unsigned reference_point_k_rb;
    /// DM-RS config type (\e dmrsConfigType).
    dmrs_type type;
    /// PDSCH DMRS-Scrambling-ID (\e pdschDmrsScramblingId).
    unsigned scrambling_id;
    /// DM-RS sequence initialization (\f$n_{SCID}\f$).
    bool n_scid;
    /// Generated signal linear amplitude.
    float amplitude;
    /// DM-RS position mask. Indicates the OFDM symbols carrying DM-RS within the slot.
    symbol_slot_mask symbols_mask;
    /// Allocation RB list, the entries set to true are used for transmission.
    crb_bitmap rb_mask;
    /// Precoding configuration.
    precoding_configuration precoding;
  };

  /// Default destructor.
  virtual ~dmrs_pdsch_processor() = default;

  /// \brief Generates and maps DMRS for PDSCH.
  /// \param [out] grid  Resource grid writer interface.
  /// \param [in] config PT-RS configuration.
  virtual void map(resource_grid_writer& grid, const config_t& config) = 0;
};

} // namespace srsran
