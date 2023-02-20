/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/static_vector.h"
#include "srsran/phy/support/resource_grid.h"
#include "srsran/ran/pci.h"

namespace srsran {

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
