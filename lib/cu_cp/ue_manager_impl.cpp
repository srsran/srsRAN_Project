/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "ue_manager_impl.h"
#include "srsran/rrc/rrc_du_factory.h"

using namespace srsran;
using namespace srs_cu_cp;

ue_manager::ue_manager(const ue_configuration& ue_config_) : ue_config(ue_config_) {}

// generic_ue_manager

ue_index_t ue_manager::get_ue_index(pci_t pci, rnti_t c_rnti)
{
  if (pci_rnti_to_ue_index.find(std::make_tuple(pci, c_rnti)) != pci_rnti_to_ue_index.end()) {
    return pci_rnti_to_ue_index.at(std::make_tuple(pci, c_rnti));
  }
  logger.debug("Ue index for pci={} and c-rnti={} not found.", pci, c_rnti);
  return ue_index_t::invalid;
}

// du_processor_ue_manager

du_ue* ue_manager::find_du_ue(ue_index_t ue_index)
{
  if (ues.find(ue_index) != ues.end() && ues.at(ue_index).du_ue_created) {
    return &ues.at(ue_index);
  }
  return nullptr;
}

du_ue* ue_manager::add_ue(du_index_t du_index, pci_t pci, rnti_t rnti)
{
  // check if PCI is valid
  if (pci == INVALID_PCI) {
    logger.error("Invalid pci={}", pci);
    return nullptr;
  }

  // check if RNTI is valid
  if (rnti == INVALID_RNTI) {
    logger.error("Invalid rnti={}", rnti);
    return nullptr;
  }

  // check if the UE is already present
  if (get_ue_index(pci, rnti) != ue_index_t::invalid) {
    logger.error("UE with pci={} and rnti={} already exists", pci, rnti);
    return nullptr;
  }

  ue_index_t new_ue_index = get_next_ue_index(du_index);
  if (new_ue_index == ue_index_t::invalid) {
    logger.error("No free ue_index available");
    return nullptr;
  }

  // Create UE object
  ues.emplace(
      std::piecewise_construct, std::forward_as_tuple(new_ue_index), std::forward_as_tuple(new_ue_index, pci, rnti));

  // Add RNTI to lookup
  pci_rnti_to_ue_index.emplace(std::make_tuple(pci, rnti), new_ue_index);

  auto& ue         = ues.at(new_ue_index);
  ue.du_ue_created = true;

  logger.debug("Added new UE with ueId={} pci={} and rnti={}", new_ue_index, pci, rnti);

  return &ue;
}

void ue_manager::remove_du_ue(ue_index_t ue_index)
{
  if (ue_index == ue_index_t::invalid) {
    logger.error("Invalid ueId={}", ue_index);
    return;
  }

  logger.debug("Scheduling ueId={} deletion", ue_index);

  if (ues.find(ue_index) == ues.end() || !ues.at(ue_index).du_ue_created) {
    logger.error("Remove UE called for inexistent ueId={}", ue_index);
    return;
  }

  // remove UE from lookups
  pci_t pci = ues.at(ue_index).get_pci();
  if (pci != INVALID_PCI) {
    rnti_t c_rnti = ues.at(ue_index).get_c_rnti();
    if (c_rnti != rnti_t::INVALID_RNTI) {
      pci_rnti_to_ue_index.erase(std::make_tuple(pci, c_rnti));
    }
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

void ue_manager::transfer_ngap_ue_context(ue_index_t new_ue_index, ue_index_t old_ue_index)
{
  // Update ue index at lookups
  ran_ue_id_to_ue_index.at(find_ran_ue_id(old_ue_index)) = new_ue_index;
  amf_ue_id_to_ue_index.at(find_amf_ue_id(old_ue_index)) = new_ue_index;

  // transfer NGAP UE Context to new UE
  auto& old_ue = ues.at(old_ue_index);
  auto& new_ue = ues.at(new_ue_index);

  new_ue.set_ngap_ue_context(old_ue.get_ngap_ue_context());

  logger.debug(
      "Transferred NGAP UE context from ueId={} (ran_ue_id={} amf_ue_id={}) to ueId={} (ran_ue_id={} amf_ue_id={})",
      old_ue_index,
      old_ue.get_ran_ue_id(),
      old_ue.get_amf_ue_id(),
      new_ue_index,
      new_ue.get_ran_ue_id(),
      new_ue.get_amf_ue_id());

  // Mark NGAP UE as removed
  ues.at(old_ue_index).ngap_ue_created = false;
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
  for (auto const& it : ran_ue_id_to_ue_index) {
    if (it.second == ue_index) {
      return it.first;
    }
  }
  logger.error("RAN UE ID for ue_index={} not found", ue_index);
  return ran_ue_id_t::invalid;
}

amf_ue_id_t ue_manager::find_amf_ue_id(ue_index_t ue_index)
{
  for (auto const& it : amf_ue_id_to_ue_index) {
    if (it.second == ue_index) {
      return it.first;
    }
  }
  logger.error("AMF UE ID for ue_index={} not found", ue_index);
  return amf_ue_id_t::invalid;
}