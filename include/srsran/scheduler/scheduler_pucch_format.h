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

#include "srsran/ran/pucch/pucch_configuration.h"
#include "srsran/ran/pucch/pucch_mapping.h"
#include "srsran/ran/resource_allocation/ofdm_symbol_range.h"
#include "srsran/ran/resource_allocation/rb_interval.h"
#include "srsran/ran/rnti.h"

namespace srsran {

/// Labels that code how many bits are used for SR.
/// For PUCCH Format 0-1, valid values: no_sr = no SR occasion; one = SR occasion.
/// For PUCCH Format 2-3-4, all possible values are valid.
enum class sr_nof_bits : unsigned { no_sr = 0, one, two, three, four };

/// Converts \ref sr_nof_bits into unsigned.
inline unsigned sr_nof_bits_to_uint(sr_nof_bits sr_bits)
{
  return static_cast<unsigned>(sr_bits);
}

/// Implements the + operator for \ref sr_nof_bits.
inline sr_nof_bits operator+(sr_nof_bits x, sr_nof_bits y)
{
  const unsigned sum = sr_nof_bits_to_uint(x) + sr_nof_bits_to_uint(y);
  return sum > sr_nof_bits_to_uint(sr_nof_bits::four) ? sr_nof_bits::four : static_cast<sr_nof_bits>(sum);
}

/// PUCCH Format 4 spreading factor.
enum class pucch_format_4_sf { sf2, sf4 };

/// PRBs and symbols used for PUCCH resources.
struct pucch_resources {
  prb_interval      prbs;
  ofdm_symbol_range symbols;
  prb_interval      second_hop_prbs;
};

/// Scheduler output for PUCCH Format 0.
struct pucch_format_0 {
  /// \c pucch-GroupHopping, as per TS 38.331.
  pucch_group_hopping group_hopping;
  /// \f$n_{ID}\f$ as per Section 6.3.2.2.1, TS 38.211.
  unsigned n_id_hopping;
  /// \c initialCyclicShift, as per TS 38.331, or Section 9.2.1, TS 38.211.
  uint8_t     initial_cyclic_shift;
  sr_nof_bits sr_bits;
  unsigned    harq_ack_nof_bits;
};

/// Scheduler output for PUCCH Format 1.
struct pucch_format_1 {
  /// \c pucch-GroupHopping, as per TS 38.331.
  pucch_group_hopping group_hopping;
  /// \f$n_{ID}\f$ as per Section 6.3.2.2.1, TS 38.211.
  unsigned n_id_hopping;
  /// \c initialCyclicShift, as per TS 38.331, or Section 9.2.1, TS 38.211.
  uint8_t initial_cyclic_shift;
  /// \c timeDomainOCC as per TS 38.331, or equivalent to index \f$n\f$ in Table 6.3.2.4.1-2, TS 38.211.
  uint8_t                  time_domain_occ;
  sr_nof_bits              sr_bits;
  unsigned                 harq_ack_nof_bits;
  pucch_repetition_tx_slot slot_repetition;
};

/// Scheduler output for PUCCH Format 2.
struct pucch_format_2 {
  sr_nof_bits sr_bits;
  unsigned    harq_ack_nof_bits;
  unsigned    csi_part1_bits;
  /// \f$n_{ID}\f$ as per Section 6.3.2.5.1 and 6.3.2.6.1, TS 38.211.
  uint16_t n_id_scambling;
  /// \f$N_{ID}^0\f$ as per TS 38.211, Section 6.4.1.3.2.1.
  uint16_t            n_id_0_scrambling;
  max_pucch_code_rate max_code_rate;
};

/// Scheduler output for PUCCH Format 3.
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

/// Scheduler output for PUCCH Format 4.
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
  /// TODO: check if this corresponds to \ref pucch_f4_occ_len.
  pucch_format_4_sf n_sf_pucch_f4;

  /// DMRS parameters.
  bool     additional_dmrs;
  uint16_t n_id_0_scrambling;
  uint16_t m_0_cyclic_shift;
};

} // namespace srsran
