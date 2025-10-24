/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/scheduler/config/scheduler_expert_config.h"

namespace srsran {
namespace config_helpers {

inline scheduler_expert_config make_default_scheduler_expert_config()
{
  scheduler_expert_config cfg;

  // The UE is not expected to decode a PDSCH scheduled with P-RNTI, RA-RNTI, SI-RNTI and Qm > 2 i.e. MCS index of 9 in
  // Table 5.1.3.1-1 of TS 38.214.
  // Note: A MCS index of 7 can handle Msg4 of size 458 bytes for PDSCH occupying symbols (2,..,14].
  // Note: A MCS index of 8 can handle Msg4 of size 325 bytes for PDSCH occupying symbols (3,..,14].
  cfg.ue.max_msg4_mcs                            = 9;
  cfg.ue.initial_ul_sinr                         = 5;
  cfg.ue.enable_csi_rs_pdsch_multiplexing        = true;
  cfg.ue.ta_measurement_slot_period              = 80;
  cfg.ue.ta_measurement_slot_prohibit_period     = 0;
  cfg.ue.ta_cmd_offset_threshold                 = 1;
  cfg.ue.ta_target                               = 0.0F;
  cfg.ue.ta_update_measurement_ul_sinr_threshold = 0.0F;

  cfg.log_broadcast_messages = true;

  return cfg;
}

} // namespace config_helpers
} // namespace srsran
