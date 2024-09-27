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
#include "srsran/ran/paging_information.h"

namespace srsran {

struct sched_paging_information {
  /// Type of Paging. RAN initiated or CN initiated.
  paging_identity_type paging_type_indicator;
  /// Paging identity assigned to UE. Possible values are \c I-RNTI-Value (Bit string of size 40) and \c NG-5G-S-TMSI
  /// (Bit string of size 48). See TS 38.331.
  uint64_t paging_identity;
  /// Cells at which to perform Paging of UE.
  static_vector<du_cell_index_t, MAX_NOF_DU_CELLS> paging_cells;
  /// UE_ID: 5G-S-TMSI mod 1024. Used by the paging scheduler to calculate the Paging Frame.
  /// \remark See TS 38.304, clause 7.1.
  unsigned ue_identity_index_value;
  /// Paging DRX cycle in radio frames.
  std::optional<unsigned> paging_drx;
};

/// Scheduler interface to handle paging a UE.
class scheduler_paging_handler
{
public:
  virtual ~scheduler_paging_handler() = default;

  /// Forward paging information to scheduler.
  virtual void handle_paging_information(const sched_paging_information& pi) = 0;
};

} // namespace srsran
