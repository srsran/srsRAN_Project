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
#include "srsgnb/ran/lcid.h"

namespace srsgnb {

/// DL Buffer state update for a given RLC bearer.
struct mac_dl_bsr_indication_message {
  du_ue_index_t ue_index;
  lcid_t        lcid;
  unsigned      bsr;
};

class mac_ue_control_information_handler
{
public:
  virtual ~mac_ue_control_information_handler() = default;

  /// Marks that the DL buffer state for a given UE logical channel needs to be recomputed.
  /// \param ue_index UE identification.
  /// \param lcid LCID of the logical channel for which the buffer state needs to be recomputed.
  virtual void handle_dl_bsr_update_required(const mac_dl_bsr_indication_message& dl_bsr) = 0;
};

} // namespace srsgnb