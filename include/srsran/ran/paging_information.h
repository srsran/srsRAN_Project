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

#include "srsran/adt/optional.h"
#include "srsran/ran/du_types.h"

namespace srsran {

/// Type of Paging. RAN initiated or CN initiated.
enum class paging_identity_type { ran_ue_paging_identity, cn_ue_paging_identity };

struct paging_information {
  /// Cells at which to perform Paging of UE.
  std::vector<du_cell_index_t> paging_cells;
  paging_identity_type         paging_type_indicator;
  /// Paging identity assigned to UE. Possible values are \c I-RNTI-Value (Bit string of size 40) and \c NG-5G-S-TMSI
  /// (Bit string of size 48). See TS 38.331.
  uint64_t paging_identity;
  /// UE_ID: 5G-S-TMSI mod 1024. Used by the paging scheduler to calculate the Paging Frame.
  /// \remark See TS 38.304, clause 7.1.
  unsigned ue_identity_index_value;
  /// Paging DRX cycle in radio frames.
  optional<unsigned> paging_drx;
  /// Values {1,...,8}. Lower value indicates higher priority.
  optional<unsigned> paging_priority;
  bool               is_paging_origin_non_3gpp_access{false};
};

} // namespace srsran
