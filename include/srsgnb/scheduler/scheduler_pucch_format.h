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

#include "srsgnb/ran/ofdm_symbol_range.h"
#include "srsgnb/ran/pucch/pucch_mapping.h"
#include "srsgnb/ran/resource_allocation/rb_interval.h"
#include "srsgnb/ran/rnti.h"

namespace srsgnb {

/// Defines whether the PUCCH within the current slot belongs to a PUCCH repetition. For more details, ref. to
/// TS 38.213, Section 9.2.6.
enum class pucch_repetition_tx_slot { no_multi_slot, starts, continues, ends };

enum class sr_nof_bits { no_sr, one, two, three, four };

enum class pucch_format_4_sf { sf2, sf4 };

struct pucch_resources {
  prb_interval      prbs;
  prb_interval      second_hop_prbs;
  ofdm_symbol_range symbols;
  bool              intra_slot_freq_hop;
};

struct pucch_format_0 {
  /// \c pucch-GroupHopping, as per TS 38.331
  pucch_group_hopping group_hopping;
  /// \f$n_{ID}\f$ as per Section 6.3.2.2.1, TS 38.211.
  unsigned n_id_hopping;
  /// \c initialCyclicShift, as per TS 38.331, or Section 9.2.1, TS 38.211.
  uint8_t     initial_cyclic_shift;
  sr_nof_bits sr_bits;
  unsigned    harq_ack_nof_bits;
};

struct pucch_format_1 {
  /// \c pucch-GroupHopping, as per TS 38.331
  pucch_group_hopping group_hopping;
  /// \f$n_{ID}\f$ as per Section 6.3.2.2.1, TS 38.211.
  unsigned n_id_hopping;
  /// \c initialCyclicShift, as per TS 38.331, or Section 9.2.1, TS 38.211.
  uint8_t                  initial_cyclic_shift;
  sr_nof_bits              sr_bits;
  unsigned                 harq_ack_nof_bits;
  pucch_repetition_tx_slot slot_repetition;
  /// \c timeDomainOCC as per TS 38.331, or equivalent to index \f$n\f$ in Table 6.3.2.4.1-2, TS 38.211. Only for PUCCH
  /// Format 1 only.
  uint8_t time_domain_occ;
};

struct pucch_format_2 {
  sr_nof_bits sr_bits;
  unsigned    harq_ack_nof_bits;
  unsigned    csi_part1_bits;
  /// \f$n_{ID}\f$ as per Section 6.3.2.5.1 and 6.3.2.6.1, TS 38.211.  For PUCCH Format 0, 1, 3, 4.
  uint16_t n_id_scambling;
  uint16_t n_id_0_scrambling;
  uint8_t  max_code_rate;
};

struct pucch_format_3 {
  /// \c pucch-GroupHopping, as per TS 38.331
  pucch_group_hopping group_hopping;
  /// \f$n_{ID}\f$ as per Section 6.3.2.2.1, TS 38.211.
  unsigned n_id_hopping;
  /// \c initialCyclicShift, as per TS 38.331, or Section 9.2.1, TS 38.211.
  uint8_t                  initial_cyclic_shift;
  sr_nof_bits              sr_bits;
  unsigned                 harq_ack_nof_bits;
  unsigned                 csi_part1_bits;
  pucch_repetition_tx_slot slot_repetition;
  /// \c timeDomainOCC as per TS 38.331, or equivalent to index \f$n\f$ in Table 6.3.2.4.1-2, TS 38.211. Only for PUCCH
  /// Format 1 only.
  uint16_t n_id_scambling;
  uint8_t  time_domain_occ;
  bool     pi_2_bpsk;
  uint8_t  max_code_rate;
  /// DMRS parameters.
  bool     additional_dmrs;
  uint16_t n_id_0_scrambling;
  uint16_t m_0_cyclic_shift;
};

struct pucch_format_4 {
  /// \c pucch-GroupHopping, as per TS 38.331
  pucch_group_hopping group_hopping;
  /// \f$n_{ID}\f$ as per Section 6.3.2.2.1, TS 38.211.
  unsigned n_id_hopping;
  /// \c initialCyclicShift, as per TS 38.331, or Section 9.2.1, TS 38.211.
  uint8_t                  initial_cyclic_shift;
  sr_nof_bits              sr_bits;
  unsigned                 harq_ack_nof_bits;
  unsigned                 csi_part1_bits;
  pucch_repetition_tx_slot slot_repetition;
  /// \c timeDomainOCC as per TS 38.331, or equivalent to index \f$n\f$ in Table 6.3.2.4.1-2, TS 38.211. Only for PUCCH
  /// Format 1 only.
  uint16_t n_id_scambling;
  uint8_t  time_domain_occ;
  bool     pi_2_bpsk;
  uint8_t  max_code_rate;

  /// Orthogonal sequence \f$n\f$, as per TS 38.211, Section 6.3.2.6.3. Only for PUCCH Format 4.
  uint8_t orthog_seq_idx;
  /// Spreading Factor \f$N_{SF}^{PUCCH,4}\f$, as per TS 38.211, Section 6.3.2.6.3. Only for PUCCH Format 4.
  pucch_format_4_sf n_sf_pucch_f4;

  /// DMRS parameters.
  bool     additional_dmrs;
  uint16_t n_id_0_scrambling;
  uint16_t m_0_cyclic_shift;
};

} // namespace srsgnb