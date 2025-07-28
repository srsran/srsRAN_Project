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

#include "srsran/adt/complex.h"
#include "srsran/adt/span.h"
#include "srsran/ran/pci.h"
#include "srsran/srsvec/sc_prod.h"

namespace srsran {

/// Primary Synchronization Signal (PSS) sequence generator.
class pss_sequence_generator
{
public:
  /// Actual sequence length.
  static constexpr unsigned sequence_length = 127;

  /// Initializes the sequence generator with the base sequence.
  pss_sequence_generator()
  {
    // Initialize M sequence x.
    std::array<uint32_t, sequence_length + 7> x;
    x[6] = 1;
    x[5] = 1;
    x[4] = 1;
    x[3] = 0;
    x[2] = 1;
    x[1] = 1;
    x[0] = 0;

    // Generate M sequence x.
    for (uint32_t i = 0; i != sequence_length; ++i) {
      x[i + 7] = (x[i + 4] + x[i]) % 2;
    }

    // Modulate M sequence d.
    for (uint32_t i = 0; i != sequence_length; ++i) {
      sequence[i] = 1.0F - 2.0F * (float)x[i];
    }
  }

  /// \brief Generates a PSS sequence according to TS38.211 Section 7.4.2.2.1.
  /// \param[out] out        Resultant complex sequence.
  /// \param[in]  pci        Physical Cell Identifier.
  /// \param[in]  amplitude  Amplitude of the output sequence.
  void generate(span<cf_t> out, pci_t pci, float amplitude) const
  {
    srsran_assert(out.size() == sequence_length,
                  "Output size (i.e., {}) must be equal to the sequence length (i.e., {}).",
                  out.size(),
                  sequence_length);
    span<const cf_t> sequence_view = sequence;
    unsigned         m             = M(phys_cell_id::NID_2(pci));

    // Copy sequence from offset to the end
    srsvec::sc_prod(out.first(sequence_length - m), sequence_view.last(sequence_length - m), amplitude);

    // Copy sequence from 0 to offset
    srsvec::sc_prod(out.last(m), sequence_view.first(m), amplitude);
  }

private:
  /// Calculates the parameter M.
  static inline unsigned M(unsigned N_id_2) { return ((43U * (N_id_2)) % sequence_length); }

  std::array<cf_t, sequence_length> sequence;
};

} // namespace srsran
