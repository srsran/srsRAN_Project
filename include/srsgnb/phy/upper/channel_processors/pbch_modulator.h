/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/adt/complex.h"
#include "srsgnb/adt/span.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/phy/constants.h"
#include "srsgnb/phy/support/resource_grid.h"
#include "srsgnb/ran/pci.h"

namespace srsgnb {

/// Describes a PBCH modulator interface
class pbch_modulator
{
public:
  /// Number of bits to modulate.
  static constexpr unsigned M_bit = 864;

  /// Number of symbols after modulation.
  static constexpr unsigned M_symb = M_bit / 2;

  /// Describes the PBCH modulator arguments.
  struct config_t {
    /// Physical cell identifier.
    pci_t phys_cell_id;
    /// SS/PBCH block index.
    unsigned ssb_idx;
    /// First subcarrier in the resource grid.
    unsigned ssb_first_subcarrier;
    /// First symbol of the SS/PBCH block within the slot.
    unsigned ssb_first_symbol;
    /// PSS linear signal amplitude.
    float amplitude;
    /// Port indexes to map the channel.
    static_vector<uint8_t, MAX_PORTS> ports;
  };

  /// Default destructor.
  virtual ~pbch_modulator() = default;

  /// \brief Modulates a PBCH message according to TS38.211 Section 7.3.3.
  /// \param[in]  bits Input bits.
  /// \param[out] grid Destination resource grid.
  /// \param[in]  config Destination resource grid.
  virtual void put(span<const uint8_t> bits, resource_grid_writer& grid, const config_t& config) = 0;
};

} // namespace srsgnb
