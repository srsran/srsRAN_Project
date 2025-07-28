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

#include "srsran/adt/static_vector.h"
#include "srsran/phy/upper/channel_estimation.h"
#include "srsran/phy/upper/dmrs_mapping.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/ran/pucch/pucch_mapping.h"
#include "srsran/ran/slot_point.h"

namespace srsran {

class resource_grid_reader;

/// Describes a DM-RS for PUCCH estimator interface.
class dmrs_pucch_estimator
{
public:
  /// Collects common parameters for all PUCCH Formats.
  struct common_configuration {
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
    /// Start PRB index, see TS38.331 PUCCH-Resource IE.
    unsigned starting_prb;
    /// \brief Index of first PRB of the second hop if intra-slot frequency hopping is enabled, empty otherwise.
    /// \remark see PUCCH-Resource IE in TS38.331.
    std::optional<unsigned> second_hop_prb;
    /// Port indices the PUCCH transmission is mapped onto.
    static_vector<uint8_t, DMRS_MAX_NPORTS> ports;
  };

  /// Collects specific PUCCH Format 2 parameters.
  struct format2_configuration : common_configuration {
    /// Number of PRBs, see PUCCH-Resource IE in TS38.331.
    unsigned nof_prb;
    /// \brief DM-RS scrambling identity, defined in TS38.211 Section 6.4.1.3.2.1.
    ///
    /// Higher layer parameter \e scramblingID0 in \e DM-RS-UplinkConfig if it is given, otherwise the physical cell
    /// identifier.
    unsigned n_id_0;
  };

  /// Collects specific PUCCH Format 3 parameters.
  struct format3_configuration : common_configuration {
    /// \brief Parameter \f$n_{ID}\f$ in TS38.211 Section 6.3.2.2.2.
    ///
    /// Higher layer parameter \e hoppingID if configured (Cell-specific scrambling ID for group hopping and sequence
    /// hopping), otherwise the physical cell identifier.
    unsigned n_id;
    /// Number of PRBs.
    unsigned nof_prb;
    /// \brief Additional DM-RS flag (true if enabled), indicating two DM-RS symbols per hop.
    /// \remark See PUCCH-Format-Config IE in TS38.331.
    bool additional_dmrs;
  };

  /// Collects specific PUCCH Format 4 parameters.
  struct format4_configuration : common_configuration {
    /// \brief Parameter \f$n_{ID}\f$ in TS38.211 Section 6.3.2.2.2.
    ///
    /// Higher layer parameter \e hoppingID if configured (Cell-specific scrambling ID for group hopping and sequence
    /// hopping), otherwise the physical cell identifier.
    unsigned n_id;
    /// \brief Additional DM-RS flag (true if enabled), indicating two DM-RS symbols per hop.
    /// \remark See PUCCH-Format-Config IE in TS38.331.
    bool additional_dmrs;
    /// Orthogonal Cover Code Index.
    unsigned occ_index;
  };

  /// Default destructor.
  virtual ~dmrs_pucch_estimator() = default;

  /// \brief Estimates the propagation channel for PUCCH Format 2.
  ///
  /// \param[out] estimate Channel estimation results.
  /// \param[in]  grid     Received resource grid.
  /// \param[in]  config   PUCCH Format 2 configuration.
  /// \note The \c estimate dimensions must be consistent with the \grid dimensions.
  virtual void
  estimate(channel_estimate& estimate, const resource_grid_reader& grid, const format2_configuration& config) = 0;

  /// \brief Estimates the propagation channel for PUCCH Format 3.
  ///
  /// \param[out] estimate Channel estimation results.
  /// \param[in]  grid     Received resource grid.
  /// \param[in]  config   PUCCH Format 3 configuration.
  /// \note The \c estimate dimensions must be consistent with the \grid dimensions.
  virtual void
  estimate(channel_estimate& estimate, const resource_grid_reader& grid, const format3_configuration& config) = 0;

  /// \brief Estimates the propagation channel for PUCCH Format 4.
  ///
  /// \param[out] estimate Channel estimation results.
  /// \param[in]  grid     Received resource grid.
  /// \param[in]  config   PUCCH Format 4 configuration.
  /// \note The \c estimate dimensions must be consistent with the \grid dimensions.
  virtual void
  estimate(channel_estimate& estimate, const resource_grid_reader& grid, const format4_configuration& config) = 0;
};

} // namespace srsran
