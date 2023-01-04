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

ue_manager::ue_manager() {}

// du_processor_ue_manager

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

size_t ue_manager::get_nof_ues()
{
  return ue_db.size();
}

// ngc_ue_manager

ngc_ue& ue_manager::add_ue(cu_cp_ue_id_t                      cu_cp_ue_id,
                           ngc_rrc_ue_pdu_notifier&           rrc_ue_pdu_notifier_,
                           ngc_rrc_ue_control_notifier&       rrc_ue_ctrl_notifier_,
                           ngc_du_processor_control_notifier& du_processor_ctrl_notifier_)
{
  uint64_t ue_id = cu_cp_ue_id_to_uint(cu_cp_ue_id);
  srsgnb_assert(not ngc_ues.contains(ue_id), "Duplicate cu_cp_ue_id={} detected", ue_id);

  ran_ue_id_t ran_ue_id = get_next_ran_ue_id();

  ngc_ues.emplace(
      ue_id, cu_cp_ue_id, ran_ue_id, rrc_ue_pdu_notifier_, rrc_ue_ctrl_notifier_, du_processor_ctrl_notifier_);

  // Add RAN UE ID to lookup
  ran_ue_id_to_cu_cp_ue_id.emplace(ran_ue_id_to_uint(ran_ue_id), cu_cp_ue_id);

  return ngc_ues[ue_id];
}

void ue_manager::remove_ue(cu_cp_ue_id_t cu_cp_ue_id)
{
  uint64_t ue_id = cu_cp_ue_id_to_uint(cu_cp_ue_id);
  srsgnb_assert(ngc_ues.contains(ue_id), "cu_cp_ue_id={} does not exist", ue_id);

  // Remove UE from lookups
  ran_ue_id_t ran_ue_id = ngc_ues[ue_id].get_ran_ue_id();
  if (ran_ue_id != ran_ue_id_t::invalid) {
    ran_ue_id_to_cu_cp_ue_id.erase(ran_ue_id_to_uint(ran_ue_id));
  }

  amf_ue_id_t amf_ue_id = ngc_ues[ue_id].get_amf_ue_id();
  if (amf_ue_id != amf_ue_id_t::invalid) {
    amf_ue_id_to_cu_cp_ue_id.erase(amf_ue_id_to_uint(amf_ue_id));
  }

  ngc_ues.erase(ue_id);
}

ngc_ue* ue_manager::find_ue(cu_cp_ue_id_t cu_cp_ue_id)
{
  std::underlying_type_t<cu_cp_ue_id_t> cu_cp_ue_id_uint = cu_cp_ue_id_to_uint(cu_cp_ue_id);
  srsgnb_assert(cu_cp_ue_id_uint < cu_cp_ue_id_to_uint(cu_cp_ue_id_t::max), "Invalid cu_cp_ue_id={}", cu_cp_ue_id_uint);
  return ngc_ues.contains(cu_cp_ue_id_uint) ? &ngc_ues[cu_cp_ue_id_uint] : nullptr;
}

void ue_manager::set_amf_ue_id(cu_cp_ue_id_t cu_cp_ue_id, amf_ue_id_t amf_ue_id)
{
  find_ue(cu_cp_ue_id)->_set_amf_ue_id(amf_ue_id);
  // Add AMF UE ID to lookup
  amf_ue_id_to_cu_cp_ue_id.emplace(amf_ue_id_to_uint(amf_ue_id), cu_cp_ue_id);
}

cu_cp_ue_id_t ue_manager::get_cu_cp_ue_id(ran_ue_id_t ran_ue_id)
{
  std::underlying_type_t<ran_ue_id_t> ran_ue_id_uint = ran_ue_id_to_uint(ran_ue_id);
  if (not ran_ue_id_to_cu_cp_ue_id.contains(ran_ue_id_uint)) {
    logger.info("UE with ran_ue_id_t={} does not exist. Dropping PDU", ran_ue_id_uint);
    return cu_cp_ue_id_t::invalid;
  }
  return ran_ue_id_to_cu_cp_ue_id[ran_ue_id_uint];
}

cu_cp_ue_id_t ue_manager::get_cu_cp_ue_id(amf_ue_id_t amf_ue_id)
{
  std::underlying_type_t<amf_ue_id_t> amf_ue_id_uint = amf_ue_id_to_uint(amf_ue_id);
  if (not amf_ue_id_to_cu_cp_ue_id.contains(amf_ue_id_uint)) {
    logger.info("UE with amf_ue_id_t={} does not exist. Dropping PDU", amf_ue_id);
    return cu_cp_ue_id_t::invalid;
  }
  return amf_ue_id_to_cu_cp_ue_id[amf_ue_id_uint];
}

// private functions

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

ran_ue_id_t ue_manager::get_next_ran_ue_id()
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

ran_ue_id_t ue_manager::find_ran_ue_id(cu_cp_ue_id_t cu_cp_ue_id)
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