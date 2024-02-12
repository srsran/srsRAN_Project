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

#include "ldpc_base_graph.h"
#include "srsran/support/units.h"

namespace srsran {

/// Collects Shared Channel (SCH) parameters derived from the transport block size and the target code rate.
struct sch_information {
  /// Codeblock CRC size.
  static constexpr units::bits CB_CRC_SIZE{24};
  /// Transport block CRC size {16, 24}.
  units::bits tb_crc_size;
  /// LDPC base graph.
  ldpc_base_graph_type base_graph;
  /// Number of codeblocks {1, ..., 52}. Parameter \f$C\f$.
  unsigned nof_cb;
  /// Number of filler bits per codeblock. Parameter \f$F\f$.
  units::bits nof_filler_bits_per_cb;
  /// LDPC lifting size. Parameter \f$Z_c\f$.
  unsigned lifting_size;
  /// Number of bits per codeblock including filler bits. Parameter \f$K\f$.
  units::bits nof_bits_per_cb;
};

/// Gets the Shared Channel (SCH) derived parameters from the transport block size and target code rate, expressed as
/// \f$R\f$.
sch_information get_sch_segmentation_info(units::bits tbs, float target_code_rate);

} // namespace srsran
