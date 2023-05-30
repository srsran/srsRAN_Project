/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "mac_dl_ue_manager.h"
#include "srsran/mac/mac_cell_control_information_handler.h"
#include "srsran/mac/mac_ue_control_information_handler.h"
#include "srsran/ran/du_types.h"

namespace srsran {

struct sched_result;

/// \brief Interface used by MAC Cell Processor to interact with the MAC scheduler.
class mac_scheduler_cell_info_handler : public mac_ue_control_information_handler
{
public:
  virtual ~mac_scheduler_cell_info_handler() = default;

  virtual const sched_result& slot_indication(slot_point slot_tx, du_cell_index_t cell_idx) = 0;
};

} // namespace srsran