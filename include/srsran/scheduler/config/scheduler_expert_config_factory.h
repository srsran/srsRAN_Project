/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
  cfg.ue.max_msg4_mcs        = 9;
  cfg.log_broadcast_messages = true;

  return cfg;
}

} // namespace config_helpers
} // namespace srsran
