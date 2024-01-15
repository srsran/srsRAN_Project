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

#include "srsran/adt/static_vector.h"
#include "srsran/phy/upper/channel_estimation.h"
#include "srsran/phy/upper/dmrs_mapping.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/ran/pucch/pucch_mapping.h"
#include "srsran/ran/slot_point.h"

namespace srsran {

class resource_grid_reader;

/// Describes a DM-RS for PUCCH processor interface.
class dmrs_pucch_processor
{
public:
  /// Describes the necessary parameters to generate DM-RS for a PUCCH transmission.
  struct config_t {
    /// PUCCH format.
    pucch_format format;
    /// Slot timing and numerology.
    slot_point slot;
    /// Cyclic Prefix.
    cyclic_prefix cp;
    /// Group and sequence hopping configuration.
    pucch_group_hopping group_hopping;
    /// Start symbol index, see TS38.331 PUCCH-Resource IE.
    unsigned start_symbol_index;
    /// Number of symbols, see TS38.331 PUCCH-Resource IE.
    unsigned nof_symbols;
    /// Start PRB index, common to all formats (see TS38.331 PUCCH-Resource IE).
    unsigned starting_prb;
    /// \brief Intra-slot frequency hopping flag (true if enabled), applicable for all PUCCH formats.
    /// \remark see PUCCH-Resource IE in TS38.331.
    bool intra_slot_hopping;
    /// Index of first PRB after PUCCH frequency hopping (see PUCCH-Resource IE in TS38.331).
    unsigned second_hop_prb;
    /// Number of PRBs, applicable for Formats 2 and 3 (see PUCCH-Resource IE in TS38.331).
    unsigned nof_prb;
    /// Initial cyclic shift, used by Formats 0 and 1 as defined in TS38.211 Section 6.3.2.2.2.
    unsigned initial_cyclic_shift;
    /// Orthogonal Cover Code Index, used by Format 1 (see PUCCH-Resource IE in TS38.331).
    unsigned time_domain_occ;
    /// \brief Additional DM-RS flag (true if enabled), indicating two DM-RS symbols per hop.
    /// \remark Applicable to PUCCH Formats 3 and 4.
    /// \remark See PUCCH-Format-Config IE in TS38.331.
    bool additional_dmrs;
    /// \brief Parameter \f$n_{ID}\f$ in TS38.211 Section 6.3.2.2.2.
    ///
    /// Higher layer parameter \e hoppingID if configured (Cell-specific scrambling ID for group hopping and sequence
    /// hopping), otherwise the physical cell identifier.
    unsigned n_id;
    /// \brief DM-RS scrambling identity, defined in TS38.211 Section 6.4.1.3.2.1.
    ///
    /// Higher layer parameter \e scramblingID0 in \e DM-RS-UplinkConfig if it is given, otherwise the physical cell
    /// identifier.
    unsigned n_id_0;
    /// Port indexes the PUCCH transmission is mapped onto.
    static_vector<uint8_t, DMRS_MAX_NPORTS> ports;
  };

  /// Default destructor.
  virtual ~dmrs_pucch_processor() = default;

  /// \brief Estimates the PUCCH propagation channel.
  ///
  /// \param[out] estimate Channel estimation results.
  /// \param[in]  grid     Received resource grid.
  /// \param[in]  config   PUCCH configuration parameters required for channel estimation.
  /// \note The \c estimate dimensions must be consistent with the \grid dimensions.
  virtual void estimate(channel_estimate& estimate, const resource_grid_reader& grid, const config_t& config) = 0;
};

} // namespace srsran
