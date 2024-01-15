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

#include "srsran/adt/bounded_bitset.h"
#include "srsran/adt/complex.h"

namespace srsran {

/// \brief Helper function to generate DMRS sequences.
///
/// This function generates DMRS sequences based on a pseudo-random sequence generation. It generates \c nof_dmrs_per_rb
/// for every active resource block given in \c rb_mask and skips the \c nof_dmrs_per_rb for every inactive resource
/// block.
///
/// \param[out] sequence Provides the destination storage for the generated sequence.
/// \param[in,out] prg Provides the pseudo-random generator to use.
/// \param[in] amplitude Provides the sequence amplitude.
/// \param[in] reference_point_k_rb Indicates the RB index for reference.
/// \param[in] nof_dmrs_per_rb Indicates the number of DMRS to generate per active RB.
/// \param[in] rb_mask Indicates the active resource blocks.
/// \note It assumes that the pseudo-random generator is initialized.
/// \note The sequence size must be consistent with the number of active RBs and the number of DMRS per RB.
inline void dmrs_sequence_generate(span<cf_t>                    sequence,
                                   pseudo_random_generator&      prg,
                                   float                         amplitude,
                                   unsigned                      reference_point_k_rb,
                                   unsigned                      nof_dmrs_per_rb,
                                   const bounded_bitset<MAX_RB>& rb_mask)
{
  unsigned nof_dmrs_symbols = nof_dmrs_per_rb * rb_mask.count();
  srsran_assert(sequence.size() == nof_dmrs_symbols,
                "DM-RS sequence size, i.e., {} does not match the number of RE allocated for DM-RS, i.e., {}.",
                sequence.size(),
                nof_dmrs_symbols);

  if (rb_mask.is_contiguous(true)) {
    // Number of PRB to skip.
    unsigned prb_skip = rb_mask.find_lowest(true) - reference_point_k_rb;
    prg.advance(prb_skip * nof_dmrs_per_rb * 2);

    prg.generate(sequence, amplitude);
    return;
  }

  // Counts consecutive used PRB.
  unsigned prb_count = 0;

  // Number of PRB to skip.
  unsigned prb_skip = 0;

  // Iterate over all PRBs, starting at reference point for k.
  for (unsigned prb_idx = reference_point_k_rb; prb_idx != rb_mask.size(); ++prb_idx) {
    // If the PRB is used for PDSCH transmission count
    if (rb_mask.test(prb_idx)) {
      // If it is the first PRB...
      if (prb_count == 0) {
        // ... discard unused pilots.
        prg.advance(prb_skip * nof_dmrs_per_rb * 2);
        prb_skip = 0;
      }
      ++prb_count;

      continue;
    }

    // Increase number of PRB to skip.
    ++prb_skip;

    // End of consecutive PRB, skip copying if no PRB was counted.
    if (prb_count == 0) {
      continue;
    }

    // Generate contiguous pilots.
    prg.generate(sequence.first(prb_count * nof_dmrs_per_rb), amplitude);

    // Advance sequence buffer.
    sequence = sequence.last(sequence.size() - prb_count * nof_dmrs_per_rb);

    // Reset counter.
    prb_count = 0;
  }

  // Generate the last group of contiguous RB.
  if (prb_count > 0) {
    prg.generate(sequence.first(prb_count * nof_dmrs_per_rb), amplitude);
  }
}

} // namespace srsran
