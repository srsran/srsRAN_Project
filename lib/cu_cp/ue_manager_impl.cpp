/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ue_manager_impl.h"
#include "srsgnb/rrc/rrc_du_factory.h"

using namespace srsran;
using namespace srs_cu_cp;

ue_manager::ue_manager() {}

// du_processor_ue_manager

du_ue* ue_manager::find_du_ue(ue_index_t ue_index)
{
  if (ues.find(ue_index) != ues.end() && ues.at(ue_index).du_ue_created) {
    return &ues.at(ue_index);
  }
  return nullptr;
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

du_ue* ue_manager::add_ue(du_index_t du_index, rnti_t rnti)
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

  ue_index_t new_ue_index = get_next_ue_index(du_index);
  if (new_ue_index == ue_index_t::invalid) {
    logger.error("No free ue_index available");
    return nullptr;
  }

  // Create UE object
  ues.emplace(std::piecewise_construct, std::forward_as_tuple(new_ue_index), std::forward_as_tuple(new_ue_index, rnti));

  // Add RNTI to lookup
  rnti_to_ue_index.emplace(rnti, new_ue_index);

  auto& ue         = ues.at(new_ue_index);
  ue.du_ue_created = true;

  logger.debug("Added new UE with ueId={} and rnti={}", new_ue_index, rnti);

  return &ue;
}

void ue_manager::remove_du_ue(ue_index_t ue_index)
{
  if (ue_index == ue_index_t::invalid) {
    logger.error("Invalid ue_index={}", ue_index);
    return;
  }

  logger.debug("Scheduling ueId={} deletion", ue_index);

  if (ues.find(ue_index) == ues.end() || !ues.at(ue_index).du_ue_created) {
    logger.error("Remove UE called for inexistent ueId={}", ue_index);
    return;
  }

  // remove UE from lookups
  rnti_t c_rnti = ues.at(ue_index).get_c_rnti();
  if (c_rnti != rnti_t::INVALID_RNTI) {
    rnti_to_ue_index.erase(c_rnti);
  }

  if (!ues.at(ue_index).ngap_ue_created) {
    // if NGAP UE was not created or already removed, remove CU-CP UE from database
    ues.erase(ue_index);
  } else {
    // Mark DU UE as removed
    ues.at(ue_index).du_ue_created = false;
  }

  logger.info("Removed ueId={}", ue_index);
  return;
}

// ngap_ue_manager

ngap_ue* ue_manager::add_ue(ue_index_t                          ue_index,
                            ngap_rrc_ue_pdu_notifier&           rrc_ue_pdu_notifier_,
                            ngap_rrc_ue_control_notifier&       rrc_ue_ctrl_notifier_,
                            ngap_du_processor_control_notifier& du_processor_ctrl_notifier_)
{
  // check if ue index is valid
  if (ue_index == ue_index_t::invalid) {
    logger.error("Invalid ue_index={}", ue_index);
    return nullptr;
  }

  // check if the UE is already present
  if (ues.find(ue_index) != ues.end() && ues.at(ue_index).ngap_ue_created) {
    logger.error("UE with ue_index={} already exists", ue_index);
    return nullptr;
  }

  // UE must be created by DU processor
  if (ues.find(ue_index) == ues.end()) {
    logger.error("UE has not been created");
    return nullptr;
  }

  ran_ue_id_t ran_ue_id = get_next_ran_ue_id();
  if (ran_ue_id == ran_ue_id_t::invalid) {
    logger.error("No free RAN UE ID available");
    return nullptr;
  }

  auto& ue = ues.at(ue_index);

  ue.set_ran_ue_id(ran_ue_id);
  ue.set_rrc_ue_pdu_notifier(rrc_ue_pdu_notifier_);
  ue.set_rrc_ue_ctrl_notifier(rrc_ue_ctrl_notifier_);
  ue.set_du_processor_ctrl_notifier(du_processor_ctrl_notifier_);

  // Add RAN UE ID to lookup
  ran_ue_id_to_ue_index.emplace(ran_ue_id, ue_index);

  ue.ngap_ue_created = true;

  logger.debug("Added NGAP context to UE (ueId={}) with ran_ue_id={}", ue_index, ran_ue_id);

  return &ue;
}

void ue_manager::remove_ngap_ue(ue_index_t ue_index)
{
  if (ue_index == ue_index_t::invalid) {
    logger.error("Invalid ue_index={}", ue_index);
    return;
  }

  logger.debug("Scheduling ueId={} deletion", ue_index);

  if (ues.find(ue_index) == ues.end() || !ues.at(ue_index).ngap_ue_created) {
    logger.error("Remove UE called for inexistent ueId={}", ue_index);
    return;
  }

  // Remove UE from lookups
  ran_ue_id_t ran_ue_id = ues.at(ue_index).get_ran_ue_id();
  if (ran_ue_id != ran_ue_id_t::invalid) {
    ran_ue_id_to_ue_index.erase(ran_ue_id);
  }

  amf_ue_id_t amf_ue_id = ues.at(ue_index).get_amf_ue_id();
  if (amf_ue_id != amf_ue_id_t::invalid) {
    amf_ue_id_to_ue_index.erase(amf_ue_id);
  }

  if (!ues.at(ue_index).du_ue_created) {
    // if DU UE was not created or already removed, remove CU-CP UE from database
    ues.erase(ue_index);
  } else {
    // Mark NGAP UE as removed
    ues.at(ue_index).ngap_ue_created = false;
  }
}

ngap_ue* ue_manager::find_ngap_ue(ue_index_t ue_index)
{
  if (ues.find(ue_index) != ues.end() && ues.at(ue_index).ngap_ue_created) {
    return &ues.at(ue_index);
  }
  return nullptr;
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

void ue_manager::set_amf_ue_id(ue_index_t ue_index, amf_ue_id_t amf_ue_id)
{
  if (ue_index == ue_index_t::invalid) {
    logger.error("Invalid ue_index={}", ue_index);
    return;
  }

  ues.at(ue_index).set_amf_ue_id(amf_ue_id);
  // Add AMF UE ID to lookup
  amf_ue_id_to_ue_index.emplace(amf_ue_id, ue_index);
}

// private functions

ue_index_t ue_manager::get_next_ue_index(du_index_t du_index)
{
  // Search unallocated UE index
  for (uint16_t i = 0; i < MAX_NOF_UES_PER_DU; i++) {
    ue_index_t new_ue_index = generate_ue_index(du_index, i);
    if (ues.find(new_ue_index) == ues.end()) {
      logger.debug("Allocating new UE index={} for du_index={}", new_ue_index, du_index);
      return new_ue_index;
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