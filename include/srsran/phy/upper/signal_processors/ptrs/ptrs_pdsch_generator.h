/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/static_vector.h"
#include "srsran/phy/support/precoding_configuration.h"
#include "srsran/phy/support/re_pattern.h"
#include "srsran/phy/upper/dmrs_mapping.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/ran/ptrs/ptrs.h"
#include "srsran/ran/ptrs/ptrs_constants.h"
#include "srsran/ran/rnti.h"
#include "srsran/ran/slot_point.h"

namespace srsran {

class resource_grid_mapper;

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
    /// Indicates the generated signal linear amplitude.
    float amplitude;
    /// DM-RS position mask. Indicates the OFDM symbols carrying DM-RS within the slot.
    symbol_slot_mask dmrs_symbols_mask;
    /// PDSCH frequency domain allocation as RB list. The entries set to true are used for transmission.
    bounded_bitset<MAX_NOF_PRBS> rb_mask;
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
  /// \param [out] mapper Destination resource grid.
  /// \param [in]  config Required configuration to generate and map the signal.
  virtual void generate(resource_grid_mapper& mapper, const configuration& config) = 0;
};

} // namespace srsran