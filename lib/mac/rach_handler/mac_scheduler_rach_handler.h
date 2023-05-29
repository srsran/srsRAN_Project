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

#include "srsran/mac/mac_cell_rach_handler.h"
#include "srsran/ran/du_types.h"

namespace srsran {

class mac_scheduler_rach_handler
{
public:
  virtual ~mac_scheduler_rach_handler() = default;

  virtual void handle_rach_indication(du_cell_index_t cell_index, const mac_rach_indication& rach_ind) = 0;
};

} // namespace srsran