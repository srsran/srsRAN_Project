
#ifndef SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PUCCH_HELPER_H
#define SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PUCCH_HELPER_H

#include "srsgnb/phy/upper/signal_processors/dmrs_pucch_processor.h"
#include "srsgnb/phy/upper/sequence_generators/pseudo_random_generator.h"
#include <srsgnb/ran/frame_types.h>

namespace srsgnb {

// Implements TS 38.211 table 6.4.1.3.1.1-1: Number of DM-RS symbols and the corresponding N_PUCCH...
unsigned pucch_dmrs_symbols(const dmrs_pucch_processor::config_t& config, unsigned m_prime)
{
  if (config.intra_slot_hopping) {
    if (m_prime == 0) {
      switch (config.nof_symbols) {
        case 4:
        case 5:
          return 1;
        case 6:
        case 7:
        case 8:
        case 9:
          return 2;
        case 10:
        case 11:
        case 12:
        case 13:
          return 3;
        case 14:
          return 4;
        default:; // Do nothing
      }
    } else {
      switch (config.nof_symbols) {
        case 4:
        case 6:
          return 1;
        case 5:
        case 7:
        case 8:
        case 10:
          return 2;
        case 9:
        case 11:
        case 12:
        case 14:
          return 3;
        case 13:
          return 4;
        default:; // Do nothing
      }
    }
  } else if (m_prime == 0) {
    switch (config.nof_symbols) {
      case 4:
        return 2;
      case 5:
      case 6:
        return 3;
      case 7:
      case 8:
        return 4;
      case 9:
      case 10:
        return 5;
      case 11:
      case 12:
        return 6;
      case 13:
      case 14:
        return 7;
      default:; // Do nothing
    }
  }
  srsran_assert(false, "Invalid case nof_symbols={} and m_prime={}", config.nof_symbols, m_prime);
  return 0;
}

/// Computes the NR-PUCCH group sequence (TS 38.211 clause 6.3.2.2.1 Group and sequence hopping).
void pucch_group_sequence(pucch_group_hopping group_hopping, unsigned n_id, unsigned& u, unsigned& v)
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
/// \param alpha_idx[out] NR alpha index
/// \param slot[in]       Current slot
/// \param n_id[in]       Higher layer parameter hopingID if configured, physical cell id otherwise
/// \param symbol[in]     OFDM symbol index in the slot
/// \param m0             Initial cyclic shift
/// \param m_cs           Cyclic shift
void pucch_alpha_index(unsigned&         alpha_idx,
                       const slot_point& slot,
                       unsigned          n_id,
                       unsigned          symbol,
                       unsigned          m0,
                       unsigned          m_cs)
{
  // Pseudo-random sequence generator instance
  std::unique_ptr<pseudo_random_generator> prg = create_pseudo_random();

  // Pseudo-random sequence length for max numerology (i.e. 4)
  const size_t seq_length = 8U * NOF_OFDM_SYM_PER_SLOT_NORMAL_CP * NOF_SUBFRAMES_PER_FRAME * (1U << 4U);

  // Initialize pseudo-random sequence with the seed set to nid
  std::array<uint8_t, seq_length> cs    = {};
  unsigned                        cinit = n_id;

  prg->init(cinit);
  prg->apply_xor_bit(cs, cs);

  // Slot number
  unsigned n_slot = slot.slot_index();

  // Create n_cs parameter
  unsigned n_cs     = 0;
  unsigned base_idx = 8U * NOF_OFDM_SYM_PER_SLOT_NORMAL_CP * n_slot + 8U * symbol;

  for (uint32_t m = 0; m < 8; m++) {
    n_cs += cs[base_idx + m] << m;
  }

  alpha_idx = (m0 + m_cs + n_cs) % NRE;
}

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PUCCH_HELPER_H
