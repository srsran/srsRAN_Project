/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

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
  std::optional<unsigned> paging_drx;
  /// Values {1,...,8}. Lower value indicates higher priority.
  std::optional<unsigned> paging_priority;
  bool                    is_paging_origin_non_3gpp_access{false};
};

} // namespace srsran
