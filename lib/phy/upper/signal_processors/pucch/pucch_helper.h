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

#include "srsran/phy/constants.h"
#include "srsran/phy/upper/sequence_generators/low_papr_sequence_collection.h"
#include "srsran/phy/upper/sequence_generators/pseudo_random_generator.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/ran/frame_types.h"
#include "srsran/ran/pucch/pucch_mapping.h"
#include "srsran/ran/slot_point.h"
#include "srsran/support/error_handling.h"

namespace srsran {

class pucch_helper
{
private:
  // Pseudo-random sequence generator instance
  std::unique_ptr<pseudo_random_generator> prg;

public:
  pucch_helper(std::unique_ptr<pseudo_random_generator> prg_) : prg(std::move(prg_))
  {
    srsran_assert(prg, "Invalid PRG.");
  }

  /// Computes the NR-PUCCH group sequence (TS 38.211 clause 6.3.2.2.1 Group and sequence hopping).
  void compute_group_sequence(pucch_group_hopping group_hopping, unsigned n_id, unsigned& u, unsigned& v)
  {
    unsigned f_gh = 0;
    unsigned f_ss = 0;

    switch (group_hopping) {
      case pucch_group_hopping::NEITHER:
        f_ss = n_id % low_papr_sequence_collection::NOF_GROUPS;
        break;
      case pucch_group_hopping::ENABLE:
        srsran_terminate("Group hopping is not implemented");
        return;
      case pucch_group_hopping::DISABLE:
        srsran_terminate("Hopping is not implemented");
        return;
    }

    u = (f_gh + f_ss) % low_papr_sequence_collection::NOF_GROUPS;
    v = 0;
  }

  /// \brief Computes the NR alpha index (1-NRE) (TS 38.211 clause 6.3.2.2.2 Cyclic shift hopping)
  ///
  /// \param slot[in]    Current slot
  /// \param cp[in]      Cyclic prefix type
  /// \param n_id[in]    Higher layer parameter hoppingID if configured, physical cell id otherwise
  /// \param symbol[in]  OFDM symbol index in the slot
  /// \param m0[in]      Initial cyclic shift
  /// \param m_cs[in]    Cyclic shift
  /// \return NR alpha index
  unsigned get_alpha_index(const slot_point&    slot,
                           const cyclic_prefix& cp,
                           unsigned             n_id,
                           unsigned             symbol,
                           unsigned             m0,
                           unsigned             m_cs) const
  {
    // Initialize pseudo-random sequence with the seed set to nid
    unsigned cinit = n_id;

    // Pseudo-random sequence length for max numerology (i.e. 4)
    const size_t max_seq_length  = 8U * MAX_NSYMB_PER_SLOT * NOF_SUBFRAMES_PER_FRAME * (1U << 4U);
    unsigned     sequence_length = 8U * get_nsymb_per_slot(cp) * slot.nof_slots_per_frame();

    // Create a zero array
    static_bit_buffer<max_seq_length> cs(sequence_length);

    prg->init(cinit);
    prg->generate(cs);

    // Slot number
    unsigned n_slot = slot.slot_index();

    // Create n_cs parameter
    unsigned base_idx = get_nsymb_per_slot(cp) * n_slot + symbol;
    unsigned n_cs     = reverse_byte(cs.get_byte(base_idx));

    unsigned alpha_idx = (m0 + m_cs + n_cs) % NRE;
    return alpha_idx;
  }
};

} // namespace srsran
