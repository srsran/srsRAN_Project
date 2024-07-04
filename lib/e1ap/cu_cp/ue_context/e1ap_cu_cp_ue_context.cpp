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

#include "e1ap_cu_cp_ue_context.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;
using namespace srsran::srs_cu_cp;

e1ap_ue_context* e1ap_ue_context_list::add_ue(ue_index_t ue_index, gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id)
{
  srsran_assert(ue_index != ue_index_t::invalid, "Invalid ue_index={}", ue_index);
  srsran_assert(cu_cp_ue_e1ap_id != gnb_cu_cp_ue_e1ap_id_t::invalid, "Invalid cu_cp_ue_e1ap_id={}", cu_cp_ue_e1ap_id);

  if (ue_index_to_ue_e1ap_id.find(ue_index) != ue_index_to_ue_e1ap_id.end()) {
    logger.error("ue={}: UE already exists", ue_index);
    return nullptr;
  }

  auto ret = ues.emplace(std::piecewise_construct,
                         std::forward_as_tuple(cu_cp_ue_e1ap_id),
                         std::forward_as_tuple(ue_index, cu_cp_ue_e1ap_id, timers));
  if (not ret.second) {
    logger.error("{}: Failed to create E1AP UE context", e1ap_ue_ids{ue_index, cu_cp_ue_e1ap_id});
    return nullptr;
  }
  ue_index_to_ue_e1ap_id.emplace(ue_index, cu_cp_ue_e1ap_id);

  logger.info("{}: Created new E1AP UE context", ret.first->second.ue_ids);
  return &ret.first->second;
}

void e1ap_ue_context_list::remove_ue(ue_index_t ue_index)
{
  srsran_assert(ue_index != ue_index_t::invalid, "Invalid ue_index={}", ue_index);

  auto id_it = ue_index_to_ue_e1ap_id.find(ue_index);
  if (id_it == ue_index_to_ue_e1ap_id.end()) {
    logger.warning("ue={}: E1AP UE Context not found", ue_index);
    return;
  }
  gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id = id_it->second;

  // Remove UE from lookup
  ue_index_to_ue_e1ap_id.erase(ue_index);

  auto ue_it = ues.find(cu_cp_ue_e1ap_id);
  if (ue_it == ues.end()) {
    logger.error("{}: UE context not found", e1ap_ue_ids{ue_index, cu_cp_ue_e1ap_id});
    return;
  }

  // Remove UE from the main map.
  logger.debug("{}: Removed E1AP UE context", ue_it->second.get_ue_ids());
  ues.erase(ue_it);
}

gnb_cu_cp_ue_e1ap_id_t e1ap_ue_context_list::allocate_gnb_cu_cp_ue_e1ap_id()
{
  if (ue_index_to_ue_e1ap_id.size() >= max_nof_supported_ues) {
    return gnb_cu_cp_ue_e1ap_id_t::invalid;
  }

  // Check if the next_cu_cp_ue_e1ap_id is available
  if (ues.find(next_cu_cp_ue_e1ap_id) == ues.end()) {
    gnb_cu_cp_ue_e1ap_id_t ret = next_cu_cp_ue_e1ap_id;
    // increase the next cu-cp ue e1ap id
    increase_next_cu_cp_ue_e1ap_id();
    return ret;
  }

  // Find holes in the allocated IDs by iterating over all ids starting with the next_cu_cp_ue_e1ap_id to find the
  // available id
  while (true) {
    // Only iterate over ue_index_to_ue_e1ap_id (size=MAX NOF CU UEs)
    // to avoid iterating over all possible values of gnb_cu_cp_ue_e1ap_id_t (size=2^32-1)
    auto it = std::find_if(ue_index_to_ue_e1ap_id.begin(), ue_index_to_ue_e1ap_id.end(), [this](auto& u) {
      return u.second == next_cu_cp_ue_e1ap_id;
    });

    // return the id if it is not already used
    if (it == ue_index_to_ue_e1ap_id.end()) {
      gnb_cu_cp_ue_e1ap_id_t ret = next_cu_cp_ue_e1ap_id;
      // increase the next cu-cp ue e1ap id
      increase_next_cu_cp_ue_e1ap_id();
      return ret;
    }

    // increase the next cu-cp ue e1ap id and try again
    increase_next_cu_cp_ue_e1ap_id();
  }

  return gnb_cu_cp_ue_e1ap_id_t::invalid;
}

void e1ap_ue_context_list::update_ue_index(ue_index_t new_ue_index, ue_index_t old_ue_index)
{
  srsran_assert(new_ue_index != ue_index_t::invalid, "Invalid new_ue_index={}", new_ue_index);
  srsran_assert(old_ue_index != ue_index_t::invalid, "Invalid old_ue_index={}", old_ue_index);
  // no need to update if the ue indexes are equal
  if (new_ue_index == old_ue_index) {
    return;
  }

  // Fetch CU-CP-UE-E1AP-ID.
  auto id_it = ue_index_to_ue_e1ap_id.find(old_ue_index);
  srsran_assert(id_it != ue_index_to_ue_e1ap_id.end(), "ue={}: GNB-CU-CP-UE-E1AP-ID not found", old_ue_index);
  gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id = id_it->second;

  auto ue_it = ues.find(cu_cp_ue_e1ap_id);
  srsran_sanity_check(ue_it != ues.end(), "cu_cp_ue_e1ap_id={}: UE context not found", cu_cp_ue_e1ap_id);
  e1ap_ue_context& ue = ue_it->second;

  // Update UE context
  ue.ue_ids.ue_index = new_ue_index;

  // Update lookup
  ue_index_to_ue_e1ap_id.erase(old_ue_index);
  ue_index_to_ue_e1ap_id.emplace(new_ue_index, cu_cp_ue_e1ap_id);

  // Update logger
  ue.logger.set_prefix({ue.ue_ids.ue_index, ue.ue_ids.cu_cp_ue_e1ap_id, ue.ue_ids.cu_up_ue_e1ap_id});
  ue.logger.log_debug("Updated UE index from ue_index={}", old_ue_index);
}

void e1ap_ue_context_list::increase_next_cu_cp_ue_e1ap_id()
{
  if (next_cu_cp_ue_e1ap_id == gnb_cu_cp_ue_e1ap_id_t::max) {
    // reset cu-cp ue e1ap id counter
    next_cu_cp_ue_e1ap_id = gnb_cu_cp_ue_e1ap_id_t::min;
  } else {
    // increase cu-cp ue e1ap id counter
    next_cu_cp_ue_e1ap_id = int_to_gnb_cu_cp_ue_e1ap_id(gnb_cu_cp_ue_e1ap_id_to_uint(next_cu_cp_ue_e1ap_id) + 1);
  }
}
