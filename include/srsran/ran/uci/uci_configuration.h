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

#include "srsran/adt/static_vector.h"
#include "srsran/adt/tiny_optional.h"
#include "srsran/adt/variant.h"
#include <cstdint>

namespace srsran {

/// \c scaling values options, part of \c UCI-OnPUSCH, part of \c PUSCH-Config, as per TS 38.331.
enum class alpha_scaling_opt { f0p5, f0p65, f0p8, f1 };

inline float alpha_scaling_to_float(alpha_scaling_opt val)
{
  switch (val) {
    case alpha_scaling_opt::f0p5:
      return 0.5f;
    case alpha_scaling_opt::f0p65:
      return 0.65f;
    case alpha_scaling_opt::f0p8:
      return 0.8f;
    case alpha_scaling_opt::f1:
      return 1.0f;
    default:
      // NOTE: This is not in the specs, but it is to prevent the compiler from flagging a warning for reaching the end
      // of a non-void function without return statement.
      srsran_assert(val > alpha_scaling_opt::f1, "Invalid Alpha scaling option");
      return 1.0f;
  }
}

/// \c BetaOffsets, as per TS 38.331. These values are mapped into floats as per Tables in Section 9.3, TS 38.213.
struct beta_offsets {
  /// \remark Apply to all members. For values larger than 31 mean, treat the parameter as "not present".
  static constexpr uint8_t invalid_beta_offset = 32;

  using uint_opt = tiny_optional<uint8_t, invalid_beta_offset>;

  /// Values {0,...,15}, as per Table 9.3-1, TS 38.213.
  /// \remark As per \c BetaOffsets, TS 38.331, {16,...,31} are accepted but treated as "reserved".
  uint_opt beta_offset_ack_idx_1;
  uint_opt beta_offset_ack_idx_2;
  uint_opt beta_offset_ack_idx_3;
  /// Values {0,...,18}, as per Table 9.3-2, TS 38.213.
  /// \remark As per \c BetaOffsets, TS 38.331, {19,...,31} are accepted but treated as "reserved".
  uint_opt beta_offset_csi_p1_idx_1;
  uint_opt beta_offset_csi_p1_idx_2;
  uint_opt beta_offset_csi_p2_idx_1;
  uint_opt beta_offset_csi_p2_idx_2;

  bool operator==(const beta_offsets& rhs) const
  {
    return beta_offset_ack_idx_1 == rhs.beta_offset_ack_idx_1 and beta_offset_ack_idx_2 == rhs.beta_offset_ack_idx_2 and
           beta_offset_ack_idx_3 == rhs.beta_offset_ack_idx_3 and
           beta_offset_csi_p1_idx_1 == rhs.beta_offset_csi_p1_idx_1 and
           beta_offset_csi_p1_idx_2 == rhs.beta_offset_csi_p1_idx_2 and
           beta_offset_csi_p2_idx_1 == rhs.beta_offset_csi_p2_idx_1 and
           beta_offset_csi_p2_idx_2 == rhs.beta_offset_csi_p2_idx_2;
  }
  bool operator!=(const beta_offsets& rhs) const { return !(rhs == *this); }
};

/// \c UCI-OnPUSCH, part of \c PUSCH-Config, as per TS 38.331.
struct uci_on_pusch {
  static constexpr size_t max_nof_beta_offsets = 4;

  using beta_offsets_semi_static = beta_offsets;
  using beta_offsets_dynamic     = static_vector<beta_offsets, max_nof_beta_offsets>;

  optional<variant<beta_offsets_dynamic, beta_offsets_semi_static>> beta_offsets_cfg;
  alpha_scaling_opt                                                 scaling;

  bool operator==(const uci_on_pusch& rhs) const
  {
    return beta_offsets_cfg == rhs.beta_offsets_cfg and scaling == rhs.scaling;
  }

  bool operator!=(const uci_on_pusch& rhs) const { return !(rhs == *this); }
};

} // namespace srsran
