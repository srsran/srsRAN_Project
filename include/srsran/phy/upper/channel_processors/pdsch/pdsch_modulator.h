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

#include "srsran/adt/span.h"
#include "srsran/adt/static_vector.h"
#include "srsran/phy/support/precoding_configuration.h"
#include "srsran/phy/support/re_pattern.h"
#include "srsran/phy/upper/channel_modulation/modulation_mapper.h"
#include "srsran/phy/upper/dmrs_mapping.h"
#include "srsran/phy/upper/rb_allocation.h"
#include "srsran/ran/cyclic_prefix.h"

namespace srsran {

class resource_grid_mapper;

/// \brief Describes a PDSCH modulator interface.
///
/// The PDSCH modulator shall modulate PDSCH according to TS38.211 section 7.3.1 Physical downlink shared channel. It
/// provides two possible modulation granularities:
/// - Codeword: it modulates a codeword and maps it.
/// - Codeblock: it modulates a single codeblock from a codeword.
class pdsch_modulator
{
public:
  /// Describes the necessary parameters to modulate a PDSCH transmission.
  struct config_t {
    /// Provides \f$n_{RNTI}\f$ from TS38.211 section 7.3.1.1 Scrambling.
    uint16_t rnti;
    /// Number of contiguous PRBs allocated to the BWP {1, ..., 275}.
    unsigned bwp_size_rb;
    /// BWP start RB index from Point A {0, ..., 274}.
    unsigned bwp_start_rb;
    /// Modulation of codeword 1 (q = 0).
    modulation_scheme modulation1;
    /// Modulation of codeword 2 ( q = 1).
    modulation_scheme modulation2;
    /// Frequency domain allocation.
    rb_allocation freq_allocation;
    /// Time domain allocation within a slot: start symbol index (0...12).
    unsigned start_symbol_index;
    /// Time domain allocation within a slot: number of symbols (1...14).
    unsigned nof_symbols;
    /// DM-RS symbol positions as a mask.
    symbol_slot_mask dmrs_symb_pos;
    /// Indicates the DMRS configuration type.
    dmrs_type dmrs_config_type;
    /// Number of DMRS CDM groups without data.
    unsigned nof_cdm_groups_without_data;
    /// Provides \f$n_{ID}\f$ from TS38.211 section 7.3.1.1 Scrambling. Range is (0...1023).
    unsigned n_id;
    /// Scaling factor to apply to the resource elements according to PDSCH power allocation in TS38.214.
    float scaling;
    /// Reserved RE pattern where PDSCH is not mapped.
    re_pattern_list reserved;
    /// Precoding information for the PDSCH transmission.
    precoding_configuration precoding;
  };

  /// Default destructor.
  virtual ~pdsch_modulator() = default;

  /// \brief Modulates a PDSCH codeword according to TS38.211 section 7.3.1 Physical downlink shared channel.
  ///
  /// \param[out] mapper Resource grid mapping interface.
  /// \param[in] codewords The encoded codewords to modulate.
  /// \param[in] config configuration parameters required for PDSCH modulation.
  /// \note The number of codewords shall be consistent with the number of layers.
  /// \note The codeword length shall be consistent with the resource mapping, considering the reserved resource
  /// elements.
  virtual void modulate(resource_grid_mapper& mapper, span<const bit_buffer> codewords, const config_t& config) = 0;
};

} // namespace srsran
