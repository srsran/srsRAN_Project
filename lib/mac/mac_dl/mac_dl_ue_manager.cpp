/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "mac_dl_ue_manager.h"
#include "srsran/support/timers2.h"

using namespace srsran;

void mac_dl_ue_manager::restart_activity_timer(du_ue_index_t ue_index)
{
  std::lock_guard<std::mutex> lock(ue_mutex[ue_index]);
  if (ue_db.contains(ue_index)) {
    ue_db[ue_index].activity_timer->run();
  }
}
