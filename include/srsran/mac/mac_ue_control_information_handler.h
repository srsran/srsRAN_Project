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

#include "srsran/ran/du_types.h"
#include "srsran/ran/lcid.h"

namespace srsran {

/// DL Buffer state for a given RLC bearer.
struct mac_dl_buffer_state_indication_message {
  du_ue_index_t ue_index;
  lcid_t        lcid;
  unsigned      bs;
};

class mac_ue_control_information_handler
{
public:
  virtual ~mac_ue_control_information_handler() = default;

  /// Forwards a new DL buffer state for a given UE and logical channel to the MAC.
  /// \param dl_bs Updated DL buffer state information for a logical channel.
  virtual void handle_dl_buffer_state_update(const mac_dl_buffer_state_indication_message& dl_bs) = 0;
};

} // namespace srsran