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
  // Initialize ran_ue_id_to_ue_ngap_id with invalid values
  std::fill(ran_ue_id_to_ue_ngap_id.begin(), ran_ue_id_to_ue_ngap_id.end(), ue_ngap_id_t::invalid);
}

ngc_ue& ngc_ue_manager::add_ue(ue_ngap_id_t ue_ngap_id, ngc_rrc_ue_notifier& rrc_ue_notifier)
{
  uint64_t ue_id = ue_ngap_id_to_uint(ue_ngap_id);
  srsgnb_assert(not ues.contains(ue_id), "Duplicate ue_ngap_id={} detected", ue_id);

  ran_ue_id_t ran_ue_id = get_next_ran_ue_id();

  ues.emplace(ue_id, ue_ngap_id, ran_ue_id, rrc_ue_notifier);

  ran_ue_id_to_ue_ngap_id.emplace(ran_ue_id_to_uint(ran_ue_id), ue_ngap_id);

  return ues[ue_id];
}

void ngc_ue_manager::remove_ue(ue_ngap_id_t ue_ngap_id)
{
  uint64_t ue_id = ue_ngap_id_to_uint(ue_ngap_id);
  srsgnb_assert(ues.contains(ue_id), "ue_ngap_id={} does not exist", ue_id);

  // Remove UE from lookup
  ran_ue_id_to_ue_ngap_id[ran_ue_id_to_uint(ues[ue_id].get_ran_ue_id())] = ue_ngap_id_t::invalid;

  ues.erase(ue_id);
}

ue_ngap_id_t ngc_ue_manager::get_ue_ngap_id(std::underlying_type_t<ran_ue_id_t> ran_ue_id_uint)
{
  if (not ran_ue_id_to_ue_ngap_id.contains(ran_ue_id_uint)) {
    logger.info("UE with ran_ue_id_t={} does not exist. Dropping PDU", ran_ue_id_uint);
    return ue_ngap_id_t::invalid;
  }
  return ran_ue_id_to_ue_ngap_id[ran_ue_id_uint];
}

// private functions

ran_ue_id_t ngc_ue_manager::get_next_ran_ue_id()
{
  // Search unallocated UE index
  for (int i = 0; i < MAX_NOF_CU_UES; i++) {
    if (not ran_ue_id_to_ue_ngap_id.contains(i)) {
      return uint_to_ran_ue_id(i);
      break;
    }
  }

  logger.error("No RAN UE ID available");
  return ran_ue_id_t::invalid;
}

ran_ue_id_t ngc_ue_manager::find_ran_ue_id(ue_ngap_id_t ue_ngap_id)
{
  unsigned ran_ue_id_uint = ran_ue_id_to_uint(ran_ue_id_t::min);
  for (auto const& it : ran_ue_id_to_ue_ngap_id) {
    if (it == ue_ngap_id) {
      return uint_to_ran_ue_id(ran_ue_id_uint);
    }
    ran_ue_id_uint++;
  }
  logger.error("RAN UE ID for ue_ngap_id={} not found", ue_ngap_id);
  return ran_ue_id_t::invalid;
}