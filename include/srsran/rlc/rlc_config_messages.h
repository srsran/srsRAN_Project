/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/adt/byte_buffer.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/lcid.h"
#include <vector>

namespace srsran {

/********************************
 * 9.2.5.3 - UE Reconfiguration
 *******************************/

struct rlc_ue_reconfiguration_message {
  struct lc_ch_to_setup {
    lcid_t lcid;
    enum class rlc_mode { AM, UM } mode;
    // ...
  };

  du_cell_index_t cell_index;
  du_ue_index_t   ue_index;
};

struct rlc_ue_reconfiguration_response_message {
  du_cell_index_t cell_index;
  du_ue_index_t   ue_index;
  bool            result;
};

/********************************
 * ORAN WP8 9.2.5.5 - UE Delete
 *******************************/

struct rlc_ue_delete_message {
  du_cell_index_t cell_index;
  du_ue_index_t   ue_index;
};

struct rlc_ue_delete_response_message {
  du_cell_index_t cell_index;
  bool            result;
};

/****************************************
 * ORAN WP8 9.2.5.8 - UL RRC Message Transfer
 ***************************************/

struct ul_rrc_transfer_message {
  du_cell_index_t cell_index;
  du_ue_index_t   ue_index;
  lcid_t          lcid;
  byte_buffer     rrc_msg;
};

/**************************************************
 * ORAN WP8 9.2.5.11 - UE Reestablishment Request
 *************************************************/

struct rlc_ue_reestablishment_message {
  du_cell_index_t     cell_index;
  du_ue_index_t       ue_index;
  std::vector<lcid_t> lc_ch_ids;
};

struct rlc_ue_reestablishment_response_message {
  du_cell_index_t cell_index;
  du_ue_index_t   ue_index;
  bool            result;
};

} // namespace srsran
