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

#include "srsgnb/ran/du_types.h"

namespace srsgnb {

struct paging_indication_message {
  /// Type of Paging. RAN initiated or CN initiated.
  enum paging_identity_type { ran_ue_paging_identity, cn_ue_paging_identity } paging_type_indicator;
  /// Paging identity assigned to UE. Possible values are \c I-RNTI-Value (Bit string of size 40) and \c NG-5G-S-TMSI
  /// (Bit string of size 48). See TS 38.331.
  bounded_bitset<48> paging_identity;
  /// DU cell index.
  du_cell_index_t cell_index;
  /// UE_ID: 5G-S-TMSI mod 1024. Used by the paging scheduler to calculate the Paging Frame.
  /// \remark See TS 38.304, clause 7.1.
  unsigned ue_identity_index_value;
  /// Paging DRX cycle in radio frames.
  optional<unsigned> paging_drx;
  /// Size of the paging message to be sent to UE.
  unsigned paging_msg_size;
};

/// Scheduler interface to handle paging a UE.
class scheduler_paging_handler
{
public:
  virtual ~scheduler_paging_handler() = default;

  /// Forward paging information to scheduler.
  virtual void handle_paging_indication(const paging_indication_message& pi) = 0;
};

} // namespace srsgnb