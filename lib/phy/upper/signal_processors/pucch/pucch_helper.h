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

#include "srsgnb/phy/upper/sequence_generators/low_papr_sequence_collection.h"
#include "srsgnb/phy/upper/sequence_generators/pseudo_random_generator.h"
#include "srsgnb/ran/cyclic_prefix.h"
#include "srsgnb/ran/frame_types.h"
#include "srsgnb/ran/pucch_mapping.h"
#include "srsgnb/ran/slot_point.h"

namespace srsgnb {

class pucch_helper
{
private:
  // Pseudo-random sequence generator instance
  std::unique_ptr<pseudo_random_generator> prg;

public:
  pucch_helper(std::unique_ptr<pseudo_random_generator> prg_) : prg(std::move(prg_))
  {
    srsgnb_assert(prg, "Invalid PRG.");
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
        srsgnb_terminate("Group hopping is not implemented");
        return;
      case pucch_group_hopping::DISABLE:
        srsgnb_terminate("Hopping is not implemented");
        return;
    }

    u = (f_gh + f_ss) % low_papr_sequence_collection::NOF_GROUPS;
    v = 0;
  }

  /// \brief Computes the NR alpha index (1-NRE) (TS 38.211 clause 6.3.2.2.2 Cyclic shift hopping)
  ///
  /// \param slot[in]    Current slot
  /// \param cp[in]      Cyclic prefix type
  /// \param n_id[in]    Higher layer parameter hopingID if configured, physical cell id otherwise
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

    std::array<uint8_t, max_seq_length> cs = {};
    // view on first sequence_length elements
    span<uint8_t> cs_span(cs.data(), sequence_length);

    prg->init(cinit);
    prg->apply_xor_bit(cs_span, cs_span);

    // Slot number
    unsigned n_slot = slot.slot_index();

    // Create n_cs parameter
    unsigned n_cs     = 0;
    unsigned base_idx = 8U * get_nsymb_per_slot(cp) * n_slot + 8U * symbol;

    for (uint32_t m = 0; m < 8; m++) {
      n_cs += cs[base_idx + m] << m;
    }

    unsigned alpha_idx = (m0 + m_cs + n_cs) % NRE;
    return alpha_idx;
  }
};

} // namespace srsgnb
