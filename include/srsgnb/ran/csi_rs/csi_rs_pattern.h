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

#include "srsgnb/adt/bounded_bitset.h"
#include "srsgnb/ran/csi_rs/csi_rs_types.h"

namespace srsgnb {

/// Collects the parameters that describe a CSI-RS allocation pattern in a resource grid.
struct csi_rs_pattern_configuration {
  /// PRB where the CSI resource starts, related to the CRB 0.
  unsigned start_rb;
  /// Number of PRBs that the CSI spans.
  unsigned nof_rb;
  /// Row number of the CSI-RS location table, as defined in TS 38.211 Table 7.4.1.5.3-1.
  unsigned csi_rs_mapping_table_row;
  /// Frequency domain allocation references \f${k_0, k_1, ..., k_n}\f$.
  static_vector<unsigned, CSI_RS_MAX_NOF_K_INDEXES> freq_allocation_ref_idx;
  /// \brief The time domain location reference \f$l_0\f$.
  ///
  /// This reference is defined relative to the start of the slot, and takes values in the range {0, 1, ..., 13}.
  unsigned symbol_l0;
  /// \brief The time domain location reference \f$l_1\f$.
  ///
  /// This reference is defined relative to the start of the slot, and takes values in the range {2, 3, ..., 12}.
  unsigned symbol_l1;
  /// CDM configuration.
  csi_rs_cdm_type cdm;
  /// Frequency density configuration.
  csi_rs_freq_density_type freq_density;
  /// Number of ports.
  unsigned nof_ports;

  bool operator==(const csi_rs_pattern_configuration& rhs) const
  {
    return start_rb == rhs.start_rb && nof_rb == rhs.nof_rb &&
           csi_rs_mapping_table_row == rhs.csi_rs_mapping_table_row &&
           freq_allocation_ref_idx == rhs.freq_allocation_ref_idx && symbol_l0 == rhs.symbol_l0 &&
           symbol_l1 == rhs.symbol_l1 && cdm == rhs.cdm && freq_density == rhs.freq_density &&
           nof_ports == rhs.nof_ports;
  }
  bool operator!=(const csi_rs_pattern_configuration& rhs) const { return !(rhs == *this); }
};

/// Describes the CSI-RS pattern within a PRB for a given port.
struct csi_rs_pattern_port {
  /// Resource element mask per resource block. True entries indicate the resource elements affected by the pattern
  /// within the PRB.
  bounded_bitset<12> re_mask;
  /// Symbol mask. True entries indicate the symbols affected by the pattern with the slot.
  bounded_bitset<14> symbol_mask;
};

/// Describes the CSI-RS pattern in the resource grid
struct csi_rs_pattern {
  /// Resource block where the pattern begins in frequency domain, the range is (0, ..., 274).
  unsigned rb_begin;
  /// Resource block where the pattern ends in frequency domain (excluded), the range is (1, ..., 275).
  unsigned rb_end;
  /// Resource block index jump.
  unsigned rb_stride;
  /// Port PRB patterns.
  static_vector<csi_rs_pattern_port, CSI_RS_MAX_NOF_PORTS> prb_patterns;
  /// Get reserved pattern.
  csi_rs_pattern_port get_reserved_pattern() const
  {
    csi_rs_pattern_port result = {bounded_bitset<12>(12), bounded_bitset<14>(14)};

    // Combines all patterns.
    for (const csi_rs_pattern_port& pattern : prb_patterns) {
      result.re_mask |= pattern.re_mask;
      result.symbol_mask |= pattern.symbol_mask;
    }

    return result;
  }
};

/// Generates the CSI-RS pattern.
csi_rs_pattern get_csi_rs_pattern(const csi_rs_pattern_configuration& config);

} // namespace srsgnb