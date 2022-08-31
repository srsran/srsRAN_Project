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
#include "srsgnb/rrc/rrc_du_factory.h"

using namespace srsgnb;
using namespace srs_cu_cp;

ue_manager::ue_manager(srslog::basic_logger& logger_) : logger(logger_) {}

ue_context* ue_manager::find_ue(ue_index_t ue_index)
{
  srsgnb_assert(ue_index < MAX_NOF_UES, "Invalid ue_index={}", ue_index);
  return ue_db.contains(ue_index) ? &ue_db[ue_index] : nullptr;
}

ue_index_t ue_manager::find_ue_index(rnti_t rnti)
{
  auto it = rnti_to_ue_index.find(rnti);
  if (it == rnti_to_ue_index.end()) {
    logger.debug("UE index for rnti={} not found", rnti);
    return INVALID_UE_INDEX;
  }

  return it->second;
}

ue_context* ue_manager::add_ue(rnti_t rnti)
{
  // check if RNTI is valid
  if (rnti == INVALID_RNTI) {
    logger.error("Invalid rnti={}", rnti);
    return nullptr;
  }

  // check if the UE is already present
  if (find_ue_index(rnti) != INVALID_UE_INDEX) {
    logger.error("UE with rnti={} already exists", rnti);
    return nullptr;
  }

  ue_index_t new_idx = get_next_ue_index();
  if (new_idx == INVALID_UE_INDEX) {
    logger.error("No free ue_index available");
    return nullptr;
  }

  // Create UE object
  ue_context new_ctx{};
  ue_db.emplace(new_idx, std::move(new_ctx));
  auto& u    = ue_db[new_idx];
  u.c_rnti   = rnti;
  u.ue_index = new_idx;

  // Update RNTI -> UE index map
  rnti_to_ue_index.emplace(u.c_rnti, new_idx);

  return &u;
}

void ue_manager::remove_ue(ue_index_t ue_index)
{
  srsgnb_assert(ue_index < MAX_NOF_UES, "Invalid ueId={}", ue_index);
  logger.debug("Scheduling ueId={} deletion", ue_index);

  srsgnb_assert(ue_db.contains(ue_index), "Remove UE called for inexistent ueId={}", ue_index);

  // remove UE from RNTI -> UE index map
  rnti_to_ue_index.erase(ue_db[ue_index].c_rnti);

  // remove UE from database
  ue_db.erase(ue_index);

  logger.info("Removed ueId={}", ue_index);
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