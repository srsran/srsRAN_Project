/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

using namespace srsran;
using namespace srs_cu_cp;

ue_manager::ue_manager(const ue_configuration& ue_config_, const up_resource_manager_cfg& up_config_) :
  ue_config(ue_config_), up_config(up_config_)
{
}

// generic_ue_manager

ue_index_t ue_manager::get_ue_index(pci_t pci, rnti_t rnti)
{
  if (pci_rnti_to_ue_index.find(std::make_tuple(pci, rnti)) != pci_rnti_to_ue_index.end()) {
    return pci_rnti_to_ue_index.at(std::make_tuple(pci, rnti));
  }
  logger.debug("UE index for pci={} and rnti={} not found", pci, rnti);
  return ue_index_t::invalid;
}

void ue_manager::remove_ue(ue_index_t ue_index)
{
  if (ue_index == ue_index_t::invalid) {
    logger.warning("Can't remove UE with invalid UE index");
    return;
  }

  if (ues.find(ue_index) == ues.end()) {
    logger.warning("ue={}: Remove UE called for inexistent UE", ue_index);
    return;
  }

  // remove UE from lookups
  pci_t pci = ues.at(ue_index).get_pci();
  if (pci != INVALID_PCI) {
    rnti_t c_rnti = ues.at(ue_index).get_c_rnti();
    if (c_rnti != rnti_t::INVALID_RNTI) {
      pci_rnti_to_ue_index.erase(std::make_tuple(pci, c_rnti));
    } else {
      logger.warning("ue={}: RNTI not found", ue_index);
    }
  } else {
    logger.debug("ue={}: PCI not found", ue_index);
  }

  // Remove CU-CP UE from database
  ues.erase(ue_index);

  logger.debug("ue={}: Removed", ue_index);
  return;
}

// du_processor_ue_manager

ue_index_t ue_manager::allocate_new_ue_index(du_index_t du_index)
{
  ue_index_t new_ue_index = get_next_ue_index(du_index);
  if (new_ue_index == ue_index_t::invalid) {
    logger.warning("No free UE index available");
    return new_ue_index;
  }

  // Create UE object
  ues.emplace(
      std::piecewise_construct, std::forward_as_tuple(new_ue_index), std::forward_as_tuple(new_ue_index, up_config));

  logger.debug("ue={}: Allocated new UE index", new_ue_index);

  return new_ue_index;
}

du_ue* ue_manager::find_ue(ue_index_t ue_index)
{
  if (ues.find(ue_index) != ues.end()) {
    return &ues.at(ue_index);
  }
  return nullptr;
}

du_ue* ue_manager::add_ue(ue_index_t ue_index, pci_t pci, rnti_t rnti)
{
  srsran_assert(ue_index != ue_index_t::invalid, "Invalid ue_index={}", ue_index);
  srsran_assert(pci != INVALID_PCI, "Invalid pci={}", pci);
  srsran_assert(rnti != rnti_t::INVALID_RNTI, "Invalid rnti={}", rnti);

  // check if ue_index is in db
  if (ues.find(ue_index) == ues.end()) {
    logger.warning("ue={}: UE not found", ue_index);
    return nullptr;
  }

  // check if the UE is already present
  if (get_ue_index(pci, rnti) != ue_index_t::invalid) {
    logger.warning("UE with pci={} and rnti={} already exists", pci, rnti);
    return nullptr;
  }

  auto& ue = ues.at(ue_index);
  ue.update_du_ue(pci, rnti);

  // Add PCI and RNTI to lookup.
  pci_rnti_to_ue_index.emplace(std::make_tuple(pci, rnti), ue_index);

  logger.debug("ue={}: Updated UE with pci={} and rnti={}", ue_index, pci, rnti);

  return &ue;
}

du_ue* ue_manager::find_du_ue(ue_index_t ue_index)
{
  if (ues.find(ue_index) != ues.end()) {
    return &ues.at(ue_index);
  }
  return nullptr;
}

// ngap_ue_manager

ngap_ue* ue_manager::add_ue(ue_index_t                          ue_index,
                            ngap_rrc_ue_pdu_notifier&           rrc_ue_pdu_notifier_,
                            ngap_rrc_ue_control_notifier&       rrc_ue_ctrl_notifier_,
                            ngap_du_processor_control_notifier& du_processor_ctrl_notifier_)
{
  srsran_assert(ue_index != ue_index_t::invalid, "Invalid ue_index={}", ue_index);

  // check if the UE is already present
  if (ues.find(ue_index) != ues.end() && ues.at(ue_index).ngap_ue_created()) {
    logger.warning("ue={}: UE already exists", ue_index);
    return nullptr;
  }

  // UE must be created by DU processor
  if (ues.find(ue_index) == ues.end()) {
    logger.warning("UE has not been created");
    return nullptr;
  }

  auto& ue = ues.at(ue_index);

  ue.add_ngap_ue_context(rrc_ue_pdu_notifier_, rrc_ue_ctrl_notifier_, du_processor_ctrl_notifier_);

  logger.debug("ue={}: Added NGAP UE", ue_index);

  return &ue;
}

ngap_ue* ue_manager::find_ngap_ue(ue_index_t ue_index)
{
  if (ues.find(ue_index) != ues.end() && ues.at(ue_index).ngap_ue_created()) {
    return &ues.at(ue_index);
  }
  return nullptr;
}

// private functions

ue_index_t ue_manager::get_next_ue_index(du_index_t du_index)
{
  // Search unallocated UE index
  for (uint16_t i = 0; i < MAX_NOF_UES_PER_DU; i++) {
    ue_index_t new_ue_index = generate_ue_index(du_index, i);
    if (ues.find(new_ue_index) == ues.end()) {
      logger.debug("Allocating new ue_index={} for du_index={}", new_ue_index, du_index);
      return new_ue_index;
    }
  }
  return ue_index_t::invalid;
}
