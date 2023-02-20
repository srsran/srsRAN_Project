/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/support/stats.h"

namespace srsgnb {

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
  sample_statistics<unsigned> ldpc_decoder_stats = {};
};

} // namespace srsgnb