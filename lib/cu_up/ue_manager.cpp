/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ue_manager.h"

using namespace srsgnb;
using namespace srs_cu_up;

ue_manager::ue_manager(srslog::basic_logger& logger_, timer_manager& timers_, gtpu_demux_ctrl& ngu_demux_) :
  logger(logger_), timers(timers_), ngu_demux(ngu_demux_)
{
}

ue_context* ue_manager::find_ue(ue_index_t ue_index)
{
  srsgnb_assert(ue_index < MAX_NOF_UES, "Invalid ue_index={}", ue_index);
  return ue_db.contains(ue_index) ? ue_db[ue_index].get() : nullptr;
}

ue_context* ue_manager::add_ue()
{
  if (ue_db.size() >= MAX_NOF_UES) {
    logger.error("Can't add new UE. Max number of UEs reached.");
    return nullptr;
  }

  ue_index_t new_idx = get_next_ue_index();
  if (new_idx == INVALID_UE_INDEX) {
    logger.error("No free ue_index available");
    return nullptr;
  }

  // Create UE object
  std::unique_ptr<ue_context> new_ctx = std::make_unique<ue_context>(new_idx, logger, timers, ngu_demux);

  // add to DB
  ue_db.emplace(new_idx, std::move(new_ctx));
  return ue_db[new_idx].get();
}

void ue_manager::remove_ue(ue_index_t ue_index)
{
  logger.debug("Scheduling ue_index={} deletion", ue_index);
  srsgnb_assert(ue_db.contains(ue_index), "Remove UE called for inexistent ue_index={}", ue_index);

  // TODO: remove lookup maps

  // remove UE from database
  ue_db.erase(ue_index);

  logger.info("Removed ue_index={}", ue_index);
  return;
}

ue_index_t ue_manager::get_next_ue_index()
{
  // Search unallocated UE index
  for (int i = 0; i < MAX_NOF_UES; i++) {
    if (not ue_db.contains(i)) {
      return int_to_ue_index(i);
      break;
    }
  }
  return INVALID_UE_INDEX;
}

size_t ue_manager::get_nof_ues()
{
  return ue_db.size();
}