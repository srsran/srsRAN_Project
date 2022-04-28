
#ifndef SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PUCCH_HELPER_H
#define SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PUCCH_HELPER_H

#include "srsgnb/phy/upper/sequence_generators/pseudo_random_generator.h"
#include <srsgnb/ran/frame_types.h>

namespace srsgnb {

class pucch_helper
{
public:
  /// Computes the NR-PUCCH group sequence (TS 38.211 clause 6.3.2.2.1 Group and sequence hopping).
  static void compute_group_sequence(pucch_group_hopping group_hopping, unsigned n_id, unsigned& u, unsigned& v)
  {
    unsigned f_gh = 0;
    unsigned f_ss = 0;

    switch (group_hopping) {
      case pucch_group_hopping::NEITHER:
        f_ss = n_id % low_papr_sequence_collection::NOF_GROUPS;
        break;
      case pucch_group_hopping::ENABLE:
        srsran_assert(false, "Group hopping is not implemented");
        return;
      case pucch_group_hopping::DISABLE:
        srsran_assert(false, "Hopping is not implemented");
        return;
    }

    u = (f_gh + f_ss) % low_papr_sequence_collection::NOF_GROUPS;
    v = 0;
  }

  /// \brief Computes the NR alpha index (1-NRE) (TS 38.211 clause 6.3.2.2.2 Cyclic shift hopping)
  ///
  /// \param slot[in]       Current slot
  /// \param n_id[in]       Higher layer parameter hopingID if configured, physical cell id otherwise
  /// \param symbol[in]     OFDM symbol index in the slot
  /// \param m0[in]         Initial cyclic shift
  /// \param m_cs[in]       Cyclic shift
  /// \return NR alpha index
  static unsigned get_alpha_index(const slot_point& slot, unsigned n_id, unsigned symbol, unsigned m0, unsigned m_cs)
  {
    // Pseudo-random sequence generator instance
    std::unique_ptr<pseudo_random_generator> prg = create_pseudo_random();

    // Initialize pseudo-random sequence with the seed set to nid
    unsigned cinit = n_id;

    // Pseudo-random sequence length for max numerology (i.e. 4)
    const size_t max_seq_length  = 8U * NOF_OFDM_SYM_PER_SLOT_NORMAL_CP * NOF_SUBFRAMES_PER_FRAME * (1U << 4U);
    unsigned     sequence_length = 8U * NOF_OFDM_SYM_PER_SLOT_NORMAL_CP * slot.nof_slots_per_frame();

    std::array<uint8_t, max_seq_length> cs = {};
    // view on first sequence_length elements
    span<uint8_t> cs_span(cs.data(), sequence_length);

    prg->init(cinit);
    prg->apply_xor_bit(cs_span, cs_span);

    // Slot number
    unsigned n_slot = slot.slot_index();

    // Create n_cs parameter
    unsigned n_cs     = 0;
    unsigned base_idx = 8U * NOF_OFDM_SYM_PER_SLOT_NORMAL_CP * n_slot + 8U * symbol;

    for (uint32_t m = 0; m < 8; m++) {
      n_cs += cs[base_idx + m] << m;
    }

    unsigned alpha_idx = (m0 + m_cs + n_cs) % NRE;
    return alpha_idx;
  }
};

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PUCCH_HELPER_H
