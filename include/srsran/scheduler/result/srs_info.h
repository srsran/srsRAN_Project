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

#include "srsran/ran/resource_allocation/ofdm_symbol_range.h"
#include "srsran/ran/rnti.h"
#include "srsran/ran/srs/srs_configuration.h"
#include "srsran/scheduler/config/bwp_configuration.h"

namespace srsran {

struct srs_info {
  rnti_t                   crnti;
  const bwp_configuration* bwp_cfg;
  uint8_t                  nof_antenna_ports;
  /// Symbols used for this SRS resource, starting from \f$l_0\f$ until \f$l_0 + n^{SRS}_{symb}\f$, as per
  /// Section 6.4.1.4.1, TS 38.211.
  ofdm_symbol_range symbols;
  /// Repetition factor \f$R\f$, or \c repetitionFactor, as per \c SRS-Resource, in \c SRS-Config, TS 38.331.
  /// \remark As per TS 38.211, Section 6.4.1.4.3, the number of repetitions must be not greater than the \ref symbols
  /// length.
  srs_nof_symbols nof_repetitions;
  /// Configuration index, given by \c c_SRS, as per \c freqHopping, \c SRS-Resource, in \c SRS-Config, TS 38.331.
  /// Values {0,...,63}.
  uint8_t config_index;
  /// SRS sequence ID or \c sequenceId, as per \c SRS-Resource, in \c SRS-Config, TS 38.331.
  /// Values {0,...,1023}.
  unsigned sequence_id;
  /// Bandwidth index, given by \c b_SRS, as per \c freqHopping, \c SRS-Resource, in \c SRS-Config, TS 38.331.
  /// Values {0,...,3}.
  uint8_t bw_index;
  /// Transmission comb size, as per \c transmissionComb, in \c SRS-Config, TS 38.331, or \f$K_{TC}\f$, as per
  /// Section 6.4.1.4.1, TS 38.211.
  tx_comb_size tx_comb;
  /// Transmission comb offset, given in \c combOffset-n2 or \c combOffset-n4, \c transmissionComb, \c SRS-Resource,
  /// in \c SRS-Config, TS 38.331. Values {0, 1} if tx_comb_size == 2, {0,...,3} if tx_comb_size == 4.
  uint8_t comb_offset;
  /// Cyclic shift, given in \c cyclicShift-n2 or \c cyclicShift-n4, \c transmissionComb, \c SRS-Resource, in \c
  /// SRS-Config, TS 38.331. Values {0,...,7} if tx_comb_size == 2, {0,...,11} if tx_comb_size == 4.
  uint8_t cyclic_shift;
  /// Frequency domain position \c freqDomainPosition, \c SRS-Resource, in \c SRS-Config, TS 38.331.
  /// Values {0,...,67}.
  uint8_t freq_position;
  /// Frequency domain shift \c freqDomainShift, \c SRS-Resource, in \c SRS-Config, TS 38.331.
  /// Values {0,...,268}.
  unsigned freq_shift;
  /// Frequency hopping \c b_hop, as per \c freqHopping, \c SRS-Resource, in \c SRS-Config, TS 38.331.
  /// Values {0,...,3}.
  uint8_t                       freq_hopping;
  srs_group_or_sequence_hopping group_or_seq_hopping;
  srs_resource_type             resource_type;
  /// SRS periodicity in slots, as per \c SRS-PeriodicityAndOffset, in \c SRS-Config, TS 38.331.
  /// \remark Only applies if resource_type == periodic or resource_type == semi_persistent.
  srs_periodicity t_srs_period;
  /// SRS offset in slots, as per \c SRS-PeriodicityAndOffset, in \c SRS-Config, TS 38.331.
  /// Values { 0,...,t_srs_period - 1}.
  /// \remark Only applies if resource_type == periodic or resource_type == semi_persistent.
  unsigned t_offset;
  /// Whether the SRS should be used to obtain a normalized channel I/Q matrix.
  bool normalized_channel_iq_matrix_requested;
  /// Whether the SRS should be used to generate a positioning report.
  bool positioning_report_requested;
};

} // namespace srsran
