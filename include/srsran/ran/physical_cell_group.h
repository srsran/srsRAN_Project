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

#include "rnti.h"

namespace srsran {

/// Options for \c pdsch-HARQ-ACK-Codebook, in \c PhysicalCellGroupConfig, TS 38.331.
enum class pdsch_harq_ack_codebook { semistatic, dynamic };

/// \c PhysicalCellGroupConfig, TS 38.331.
struct physical_cell_group_config {
  /// Instead of using an optional<>, \c not_set stands for not present.
  enum class x_scale_opt { not_set, db0, db6 };

  /// Optional field with 1 possible value. False = not present.
  bool harq_ack_spatial_bundling_pucch;
  /// Optional field with 1 possible value. False = not present.
  bool harq_ack_spatial_bundling_pusch;
  /// Values {-30,...,33}.
  optional<int>           p_nr_fr1;
  pdsch_harq_ack_codebook pdsch_harq_codebook;
  optional<rnti_t>        tpc_srs_rnti;
  optional<rnti_t>        tpc_pucch_rnti;
  optional<rnti_t>        tpc_pusch_rnti;
  optional<rnti_t>        sp_csi_rnti;
  optional<rnti_t>        cs_rnti;
  optional<rnti_t>        mcs_rnti;
  /// Values {-30,...,33}.
  optional<int> p_ue_fr1;
  x_scale_opt   x_scale;
  /// Values {1,...,15}.
  optional<unsigned> pdcch_blind_detection;
};

} // namespace srsran
