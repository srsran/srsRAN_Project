/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
  /// Describes the required parameters to generate the signal as described in 3GPP TS 38.211 section 7.4.1.1.
  struct config_t {
    /// Slot context for sequence initialization.
    slot_point slot;
    /// Reference point for PDSCH DMRS \e k in RBs.
    unsigned reference_point_k_rb;
    /// DM-RS config type (\e dmrsConfigType).
    dmrs_type type;
    /// PDSCH DMRS-Scrambling-ID (\e pdschDmrsScramblingId).
    unsigned scrambling_id;
    /// DMRS sequence initialization (\f$n_{SCID}\f$).
    bool n_scid;
    /// Indicates the generated signal linear amplitude.
    float amplitude;
    /// DM-RS position mask. Indicates the OFDM symbols carrying DM-RS within the slot.
    symbol_slot_mask symbols_mask;
    /// Allocation RB list, the entries set to true are used for transmission.
    prb_bitmap rb_mask;
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
