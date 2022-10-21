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

#include "srsgnb/ran/ldpc_base_graph.h"

namespace srsgnb {

/// Collects Shared Channel (SCH) derived parameters from the transport block size and the target code rate.
struct sch_information {
  /// Codeblock CRC size.
  static constexpr unsigned CB_CRC_SIZE = 24U;
  /// Transport block CRC size {16, 24}.
  unsigned tb_crc_size;
  /// LDPC base graph selection.
  ldpc_base_graph_type base_graph;
  /// Number of codeblocks {1, ..., 52}. Parameter \f$C\f$.
  unsigned nof_cb;
  /// Number of filler bits per codeblock. Parameter \f$F\f$.
  unsigned nof_filler_bits_per_cb;
  /// LDPC lifting size. Parameter \f$Z_c\f$.
  unsigned lifting_size;
  /// Number of bits per codeblock after CRC appending. Parameter \f$K\f$.
  unsigned nof_bits_per_cb;
};

/// Gets the Shared Channel (SCH) derived parameters from the transport block size and target code rate.
sch_information get_sch_segmentation_info(unsigned tbs, float target_code_rate);

} // namespace srsgnb
