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

#include "srsran/adt/static_vector.h"
#include "srsran/phy/constants.h"
#include "srsran/ran/pci.h"

namespace srsran {

class resource_grid_writer;

/// Describes a DMRS for PBCH processor interface.
class dmrs_pbch_processor
{
public:
  /// Describes the required parameters to generate the signal.
  struct config_t {
    /// Physical cell identifier.
    pci_t phys_cell_id;
    /// SS/PBCH block index in the burst.
    unsigned ssb_idx;
    /// Maximum number of SS/PBCH transmissions in a burst (5ms).
    unsigned L_max;
    /// First subcarrier in the resource grid.
    unsigned ssb_first_subcarrier;
    /// First symbol of the SS/PBCH block within the slot.
    unsigned ssb_first_symbol;
    /// Flag: true if the SS/PBCH block transmission is in an odd half frame, false otherwise.
    bool hrf;
    /// Linear signal amplitude.
    float amplitude;
    /// Port indexes to map the signal.
    static_vector<uint8_t, MAX_PORTS> ports;
  };

  /// Default destructor.
  virtual ~dmrs_pbch_processor() = default;

  /// \brief Generates and maps DM-RS for PBCH.
  /// \param[out] grid Destination resource grid.
  /// \param[in]  config Configuration to generate and map the signal.
  virtual void map(resource_grid_writer& grid, const config_t& config) = 0;
};

} // namespace srsran
