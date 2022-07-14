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
#include "srsgnb/rrc/rrc_factory.h"

using namespace srsgnb;
using namespace srs_cu_cp;

ue_manager::ue_manager(cu_cp_manager_config_t& cfg_) : cfg(cfg_), logger(cfg.logger)
{
  std::fill(rnti_to_ue_index.begin(), rnti_to_ue_index.end(), -1);
}

ue_context* ue_manager::find_ue(ue_index_t ue_index)
{
  srsran_assert(ue_index < MAX_NOF_UES, "Invalid ue_index={}", ue_index);
  return ue_db.contains(ue_index) ? &ue_db[ue_index] : nullptr;
}

ue_context* ue_manager::find_rnti(rnti_t rnti)
{
  if (rnti_to_ue_index[rnti % MAX_NOF_UES] < 0) {
    return nullptr;
  }
  return &ue_db[rnti_to_ue_index[rnti % MAX_NOF_UES]];
}

ue_context* ue_manager::add_ue(rnti_t rnti)
{
  // check if the UE is already present
  if (find_rnti(rnti) != nullptr) {
    logger.info("UE with rnti={} already exists", rnti);
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
  auto& u = ue_db[new_idx];

  // Update RNTI -> UE index map
  srsran_sanity_check(rnti_to_ue_index[u.c_rnti % MAX_NOF_UES] < 0, "Invalid RNTI=0x{:x}", u.c_rnti);
  rnti_to_ue_index[u.c_rnti % MAX_NOF_UES] = new_idx;

  return &u;
}

void ue_manager::remove_ue(ue_index_t ue_index)
{
  srsran_assert(ue_index < MAX_NOF_UES, "Invalid ueId={}", ue_index);
  logger.debug("Scheduling ueId={} deletion", ue_index);

  srsran_assert(ue_db.contains(ue_index), "Remove UE called for inexistent ueId={}", ue_index);
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