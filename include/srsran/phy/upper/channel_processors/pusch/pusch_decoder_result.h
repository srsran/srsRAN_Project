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

#include "srsran/support/stats.h"

namespace srsran {

/// PUSCH decoding statistics.
struct pusch_decoder_result {
  /// Denotes whether the received transport block passed the CRC.
  bool tb_crc_ok = false;
  /// Total number of codeblocks in the current codeword.
  unsigned nof_codeblocks_total = 0;
  /// \brief LDPC decoding statistics.
  ///
  /// Provides access to LDPC decoding statistics such as the number of decoded codeblocks (via
  /// <tt>ldpc_stats->get_nof_observations()</tt>) or the average number of iterations for correctly decoded
  /// codeblocks (via <tt>ldpc_stats->get_mean()</tt>).
  sample_statistics<unsigned> ldpc_decoder_stats;
};

} // namespace srsran