/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ngc_ue_manager.h"

using namespace srsgnb;
using namespace srs_cu_cp;

ngc_ue_manager::ngc_ue_manager() : logger(srslog::fetch_basic_logger("NGC"))
{
  // Initialize ran_ue_id_to_cu_cp_ue_id with invalid values
  std::fill(ran_ue_id_to_cu_cp_ue_id.begin(), ran_ue_id_to_cu_cp_ue_id.end(), cu_cp_ue_id_t::invalid);
}

ngc_ue& ngc_ue_manager::add_ue(cu_cp_ue_id_t cu_cp_ue_id, ngc_rrc_ue_notifier& rrc_ue_notifier)
{
  uint64_t ue_id = cu_cp_ue_id_to_uint(cu_cp_ue_id);
  srsgnb_assert(not ues.contains(ue_id), "Duplicate cu_cp_ue_id={} detected", ue_id);

  ran_ue_id_t ran_ue_id = get_next_ran_ue_id();

  ues.emplace(ue_id, cu_cp_ue_id, ran_ue_id, rrc_ue_notifier);

  ran_ue_id_to_cu_cp_ue_id.emplace(ran_ue_id_to_uint(ran_ue_id), cu_cp_ue_id);

  return ues[ue_id];
}

void ngc_ue_manager::remove_ue(cu_cp_ue_id_t cu_cp_ue_id)
{
  uint64_t ue_id = cu_cp_ue_id_to_uint(cu_cp_ue_id);
  srsgnb_assert(ues.contains(ue_id), "cu_cp_ue_id={} does not exist", ue_id);

  // Remove UE from lookup
  ran_ue_id_to_cu_cp_ue_id[ran_ue_id_to_uint(ues[ue_id].get_ran_ue_id())] = cu_cp_ue_id_t::invalid;

  ues.erase(ue_id);
}

cu_cp_ue_id_t ngc_ue_manager::get_cu_cp_ue_id(std::underlying_type_t<ran_ue_id_t> ran_ue_id_uint)
{
  if (not ran_ue_id_to_cu_cp_ue_id.contains(ran_ue_id_uint)) {
    logger.info("UE with ran_ue_id_t={} does not exist. Dropping PDU", ran_ue_id_uint);
    return cu_cp_ue_id_t::invalid;
  }
  return ran_ue_id_to_cu_cp_ue_id[ran_ue_id_uint];
}

// private functions

ran_ue_id_t ngc_ue_manager::get_next_ran_ue_id()
{
  // Search unallocated UE index
  for (int i = 0; i < MAX_NOF_CU_UES; i++) {
    if (not ran_ue_id_to_cu_cp_ue_id.contains(i)) {
      return uint_to_ran_ue_id(i);
      break;
    }
  }

  logger.error("No RAN UE ID available");
  return ran_ue_id_t::invalid;
}

ran_ue_id_t ngc_ue_manager::find_ran_ue_id(cu_cp_ue_id_t cu_cp_ue_id)
{
  unsigned ran_ue_id_uint = ran_ue_id_to_uint(ran_ue_id_t::min);
  for (auto const& it : ran_ue_id_to_cu_cp_ue_id) {
    if (it == cu_cp_ue_id) {
      return uint_to_ran_ue_id(ran_ue_id_uint);
    }
    ran_ue_id_uint++;
  }
  logger.error("RAN UE ID for cu_cp_ue_id={} not found", cu_cp_ue_id);
  return ran_ue_id_t::invalid;
}