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
  /// Used by the paging scheduler to calculate the Paging Frame (from F1AP Paging Message).
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
  virtual void handle_paging_indication(const paging_indication_message&    pi,
                                        const std::vector<du_cell_index_t>& paging_cell_list) = 0;
};

} // namespace srsgnb