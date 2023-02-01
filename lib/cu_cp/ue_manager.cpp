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

du_ue* ue_manager::find_du_ue(ue_index_t ue_index)
{
  srsgnb_assert(ue_index != ue_index_t::invalid, "Invalid ue_index={}", ue_index);
  return du_ues.contains(ue_index_to_uint(ue_index)) ? &du_ues[ue_index_to_uint(ue_index)] : nullptr;
}

ue_index_t ue_manager::get_ue_index(rnti_t rnti)
{
  auto it = rnti_to_ue_index.find(rnti);
  if (it == rnti_to_ue_index.end()) {
    logger.debug("UE index for rnti={} not found", rnti);
    return ue_index_t::invalid;
  }

  return it->second;
}

du_ue* ue_manager::add_du_ue(rnti_t rnti)
{
  // check if RNTI is valid
  if (rnti == INVALID_RNTI) {
    logger.error("Invalid rnti={}", rnti);
    return nullptr;
  }

  // check if the UE is already present
  if (get_ue_index(rnti) != ue_index_t::invalid) {
    logger.error("UE with rnti={} already exists", rnti);
    return nullptr;
  }

  ue_index_t new_ue_index = get_next_ue_index();
  if (new_ue_index == ue_index_t::invalid) {
    logger.error("No free ue_index available");
    return nullptr;
  }

  // Create UE object
  du_ues.emplace(ue_index_to_uint(new_ue_index), new_ue_index, rnti);

  // Add RNTI to lookup
  rnti_to_ue_index.emplace(rnti, new_ue_index);

  return &du_ues[ue_index_to_uint(new_ue_index)];
}

void ue_manager::remove_du_ue(ue_index_t ue_index)
{
  srsgnb_assert(ue_index != ue_index_t::invalid, "Invalid ueId={}", ue_index);
  logger.debug("Scheduling ueId={} deletion", ue_index);

  srsgnb_assert(du_ues.contains(ue_index_to_uint(ue_index)), "Remove UE called for inexistent ueId={}", ue_index);

  // remove UE from lookups
  rnti_t c_rnti = du_ues[ue_index_to_uint(ue_index)].get_c_rnti();
  if (c_rnti != rnti_t::INVALID_RNTI) {
    rnti_to_ue_index.erase(c_rnti);
  }

  // remove UE from database
  du_ues.erase(ue_index_to_uint(ue_index));

  logger.info("Removed ueId={}", ue_index);
  return;
}

size_t ue_manager::get_nof_du_ues()
{
  return du_ues.size();
}

// ngc_ue_manager

ngc_ue& ue_manager::add_ngap_ue(ue_index_t                         ue_index,
                                ngc_rrc_ue_pdu_notifier&           rrc_ue_pdu_notifier_,
                                ngc_rrc_ue_control_notifier&       rrc_ue_ctrl_notifier_,
                                ngc_du_processor_control_notifier& du_processor_ctrl_notifier_)
{
  srsgnb_assert(ngap_ues.find(ue_index) == ngap_ues.end(), "Duplicate ue_index={} detected", ue_index);

  ran_ue_id_t ran_ue_id = get_next_ran_ue_id();

  ngap_ues.emplace(std::piecewise_construct,
                   std::forward_as_tuple(ue_index),
                   std::forward_as_tuple(
                       ue_index, ran_ue_id, rrc_ue_pdu_notifier_, rrc_ue_ctrl_notifier_, du_processor_ctrl_notifier_));

  // Add RAN UE ID to lookup
  ran_ue_id_to_ue_index.emplace(ran_ue_id, ue_index);

  return ngap_ues.at(ue_index);
}

void ue_manager::remove_ngap_ue(ue_index_t ue_index)
{
  srsgnb_assert(ngap_ues.find(ue_index) != ngap_ues.end(), "ue_index={} does not exist", ue_index);

  // Remove UE from lookups
  ran_ue_id_t ran_ue_id = ngap_ues.at(ue_index).get_ran_ue_id();
  if (ran_ue_id != ran_ue_id_t::invalid) {
    ran_ue_id_to_ue_index.erase(ran_ue_id);
  }

  amf_ue_id_t amf_ue_id = ngap_ues.at(ue_index).get_amf_ue_id();
  if (amf_ue_id != amf_ue_id_t::invalid) {
    amf_ue_id_to_ue_index.erase(amf_ue_id);
  }

  ngap_ues.erase(ue_index);
}

ngc_ue* ue_manager::find_ngap_ue(ue_index_t ue_index)
{
  srsgnb_assert(ue_index < ue_index_t::max, "Invalid ue_index={}", ue_index);
  return ngap_ues.find(ue_index) != ngap_ues.end() ? &ngap_ues.at(ue_index) : nullptr;
}

void ue_manager::set_amf_ue_id(ue_index_t ue_index, amf_ue_id_t amf_ue_id)
{
  find_ngap_ue(ue_index)->_set_amf_ue_id(amf_ue_id);
  // Add AMF UE ID to lookup
  amf_ue_id_to_ue_index.emplace(amf_ue_id, ue_index);
}

ue_index_t ue_manager::get_ue_index(ran_ue_id_t ran_ue_id)
{
  if (ran_ue_id_to_ue_index.find(ran_ue_id) == ran_ue_id_to_ue_index.end()) {
    logger.info("UE with ran_ue_id_t={} does not exist. Dropping PDU", ran_ue_id);
    return ue_index_t::invalid;
  }
  return ran_ue_id_to_ue_index[ran_ue_id];
}

ue_index_t ue_manager::get_ue_index(amf_ue_id_t amf_ue_id)
{
  if (amf_ue_id_to_ue_index.find(amf_ue_id) == amf_ue_id_to_ue_index.end()) {
    logger.info("UE with amf_ue_id_t={} does not exist. Dropping PDU", amf_ue_id);
    return ue_index_t::invalid;
  }
  return amf_ue_id_to_ue_index[amf_ue_id];
}

// private functions

ue_index_t ue_manager::get_next_ue_index()
{
  // Search unallocated UE index
  for (unsigned i = 0; i < MAX_NOF_UES_PER_DU; i++) {
    if (not du_ues.contains(i)) {
      return uint_to_ue_index(i);
      break;
    }
  }
  return ue_index_t::invalid;
}

ran_ue_id_t ue_manager::get_next_ran_ue_id()
{
  // Search unallocated UE index
  for (uint64_t i = 0; i < MAX_NOF_RAN_UES; i++) {
    ran_ue_id_t next_ran_ue_id = uint_to_ran_ue_id(i);
    if (ran_ue_id_to_ue_index.find(next_ran_ue_id) == ran_ue_id_to_ue_index.end()) {
      return next_ran_ue_id;
      break;
    }
  }

  logger.error("No RAN UE ID available");
  return ran_ue_id_t::invalid;
}

ran_ue_id_t ue_manager::find_ran_ue_id(ue_index_t ue_index)
{
  unsigned ran_ue_id_uint = ran_ue_id_to_uint(ran_ue_id_t::min);
  for (auto const& it : ran_ue_id_to_ue_index) {
    if (it.second == ue_index) {
      return uint_to_ran_ue_id(ran_ue_id_uint);
    }
    ran_ue_id_uint++;
  }
  logger.error("RAN UE ID for ue_index={} not found", ue_index);
  return ran_ue_id_t::invalid;
}