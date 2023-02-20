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
#include "srsran/phy/upper/dmrs_mapping.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/ran/slot_point.h"

namespace srsran {

/// Describes a DMRS for PDSCH processor interface.
class dmrs_pdsch_processor
{
public:
  /// Describes the required parameters to generate the signal as described in 3GPP TS 38.211 section 7.4.1.1.
  struct config_t {
    /// Slot context for sequence initialization.
    slot_point slot;
    /// Reference point for PDSCH DMRS \e k in RBs.
    unsigned reference_point_k_rb;
    /// DL DMRS config type (dmrsConfigType).
    dmrs_type type;
    /// PDSCH DMRS-Scrambling-ID (pdschDmrsScramblingId).
    unsigned scrambling_id;
    /// DMRS sequence initialization (\f$n_{SCID}\f$).
    bool n_scid;
    /// Indicates the generated signal linear amplitude.
    float amplitude;
    /// DM-RS position mask. Indicates the OFDM symbols carrying DM-RS within the slot.
    symbol_slot_mask symbols_mask;
    /// Allocation RB list, the entries set to true are used for transmission.
    bounded_bitset<MAX_RB> rb_mask;
    /// List of ports, every entry is an index.
    static_vector<uint8_t, DMRS_MAX_NPORTS> ports;
  };

  /// Default destructor.
  virtual ~dmrs_pdsch_processor() = default;

  /// \brief Generates and maps DMRS for PDSCH.
  /// \param [out] grid Provides the destination resource grid.
  /// \param [in] config Provides the required configuration to generate and map the signal.
  virtual void map(resource_grid_writer& grid, const config_t& config) = 0;
};

} // namespace srsran
