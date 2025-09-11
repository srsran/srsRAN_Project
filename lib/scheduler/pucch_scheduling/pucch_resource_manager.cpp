/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "../support/pucch/pucch_default_resource.h"
#include "pucch_allocator_impl.h"

using namespace srsran;

/////////////    RESOURCE MANAGER     /////////////

/////////////   Static function   /////////////

static int get_pucch_res_idx_for_csi(const ue_cell_configuration& ue_cell_cfg)
{
  // We assume we use only 1 CSI report.
  const unsigned csi_report_cfg_idx = 0;
  // We assume we use the First BWP.
  // TODO: extend by passing the BWP id.
  const bwp_id_t bwp_id         = MIN_BWP_ID;
  const auto&    csi_report_cfg = ue_cell_cfg.csi_meas_cfg()->csi_report_cfg_list[csi_report_cfg_idx];
  span<const csi_report_config::pucch_csi_resource> csi_pucch_res_list =
      std::get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(csi_report_cfg.report_cfg_type)
          .pucch_csi_res_list;

  for (const auto& csi : csi_pucch_res_list) {
    if (csi.ul_bwp == bwp_id) {
      return static_cast<int>(csi.pucch_res_id.cell_res_id);
    }
  }

  return -1;
}

/////////////   Public methods   /////////////

pucch_resource_manager::pucch_resource_manager()
{
  auto reset_slot_record = [](rnti_pucch_res_id_slot_record& res_counter) {
    for (auto& ue_rec : res_counter.ues_using_pucch_res) {
      ue_rec.rnti           = rnti_t::INVALID_RNTI;
      ue_rec.resource_usage = pucch_resource_usage::NOT_USED;
    }
    for (auto& res : res_counter.used_common_resources) {
      res = false;
    }
  };

  std::for_each(resource_slots.begin(), resource_slots.end(), reset_slot_record);
}

void pucch_resource_manager::slot_indication(slot_point slot_tx)
{
  // If last_sl_ind is not valid (not initialized), then the check sl_tx == last_sl_ind + 1 does not matter.
  srsran_sanity_check(not last_sl_ind.valid() or slot_tx == last_sl_ind + 1, "Detected a skipped slot");

  // Update Slot.
  last_sl_ind = slot_tx;

  rnti_pucch_res_id_slot_record& res_counter = get_slot_resource_counter(last_sl_ind - 1);

  for (auto& ue_rec : res_counter.ues_using_pucch_res) {
    ue_rec.rnti           = rnti_t::INVALID_RNTI;
    ue_rec.resource_usage = pucch_resource_usage::NOT_USED;
  }
  for (auto& res : res_counter.used_common_resources) {
    res = false;
  }
}

void pucch_resource_manager::stop()
{
  for (auto& res_counter : resource_slots) {
    for (auto& ue_rec : res_counter.ues_using_pucch_res) {
      ue_rec.rnti           = rnti_t::INVALID_RNTI;
      ue_rec.resource_usage = pucch_resource_usage::NOT_USED;
    }
    for (auto& res : res_counter.used_common_resources) {
      res = false;
    }
  }
  last_sl_ind = {};
}

bool pucch_resource_manager::is_common_resource_available(slot_point sl, size_t r_pucch)
{
  srsran_assert(r_pucch < 16, "r_PUCCH must be less than 16");
  return not get_slot_resource_counter(sl).used_common_resources[r_pucch];
}

void pucch_resource_manager::reserve_common_resource(slot_point sl, size_t r_pucch)
{
  srsran_assert(r_pucch < 16, "r_PUCCH must be less than 16");
  get_slot_resource_counter(sl).used_common_resources[r_pucch] = true;
}

pucch_harq_resource_alloc_record
pucch_resource_manager::reserve_next_set_0_harq_res_available(slot_point          slot_harq,
                                                              rnti_t              crnti,
                                                              const pucch_config& pucch_cfg)
{
  return reserve_next_harq_res_available(slot_harq, crnti, pucch_cfg, pucch_res_set_idx::set_0);
}

pucch_harq_resource_alloc_record
pucch_resource_manager::reserve_next_set_1_harq_res_available(slot_point          slot_harq,
                                                              rnti_t              crnti,
                                                              const pucch_config& pucch_cfg)
{
  return reserve_next_harq_res_available(slot_harq, crnti, pucch_cfg, pucch_res_set_idx::set_1);
}

const pucch_resource* pucch_resource_manager::reserve_set_0_res_by_res_indicator(slot_point          slot_harq,
                                                                                 rnti_t              crnti,
                                                                                 unsigned            res_indicator,
                                                                                 const pucch_config& pucch_cfg)
{
  return reserve_harq_res_by_res_indicator(slot_harq, crnti, res_indicator, pucch_cfg, pucch_res_set_idx::set_0);
}

const pucch_resource* pucch_resource_manager::reserve_set_1_res_by_res_indicator(slot_point          slot_harq,
                                                                                 rnti_t              crnti,
                                                                                 unsigned            res_indicator,
                                                                                 const pucch_config& pucch_cfg)
{
  return reserve_harq_res_by_res_indicator(slot_harq, crnti, res_indicator, pucch_cfg, pucch_res_set_idx::set_1);
}

const pucch_resource* pucch_resource_manager::reserve_csi_resource(slot_point                   slot_csi,
                                                                   rnti_t                       crnti,
                                                                   const ue_cell_configuration& ue_cell_cfg)
{
  srsran_sanity_check(slot_csi < last_sl_ind + RES_MANAGER_RING_BUFFER_SIZE,
                      "PUCCH being allocated too far into the future");

  // Get CSI specific PUCCH resource ID from the CSI meas config.
  const int csi_pucch_res_idx_int = get_pucch_res_idx_for_csi(ue_cell_cfg);
  if (csi_pucch_res_idx_int < 0) {
    return nullptr;
  }
  const auto csi_pucch_res_idx = static_cast<unsigned>(csi_pucch_res_idx_int);

  // Get resource list of wanted slot.
  auto& slot_record = get_slot_resource_counter(slot_csi);
  if (csi_pucch_res_idx >= slot_record.ues_using_pucch_res.size() or
      (slot_record.ues_using_pucch_res[csi_pucch_res_idx].rnti != rnti_t::INVALID_RNTI and
       slot_record.ues_using_pucch_res[csi_pucch_res_idx].rnti != crnti)) {
    return nullptr;
  }

  span<const pucch_resource> pucch_res_list = ue_cell_cfg.init_bwp().ul_ded->pucch_cfg.value().pucch_res_list;
  // Search for the PUCCH resource with the correct PUCCH resource ID from the PUCCH resource list.
  const auto* res_cfg =
      std::find_if(pucch_res_list.begin(), pucch_res_list.end(), [csi_pucch_res_idx](const pucch_resource& res) {
        return res.res_id.cell_res_id == csi_pucch_res_idx;
      });

  if (res_cfg == pucch_res_list.end()) {
    return nullptr;
  }

  // If the PUCCH res with correct ID was not allocated to the UE's RNTI, allocate it to this RNTI; otherwise, it means
  // the resource had already been allocated, just return it.
  if (slot_record.ues_using_pucch_res[csi_pucch_res_idx].rnti != crnti) {
    slot_record.ues_using_pucch_res[csi_pucch_res_idx].rnti           = crnti;
    slot_record.ues_using_pucch_res[csi_pucch_res_idx].resource_usage = pucch_resource_usage::CSI;
  }

  return res_cfg;
}

const pucch_resource*
pucch_resource_manager::reserve_sr_res_available(slot_point slot_sr, rnti_t crnti, const pucch_config& pucch_cfg)
{
  srsran_sanity_check(slot_sr < last_sl_ind + RES_MANAGER_RING_BUFFER_SIZE,
                      "PUCCH being allocated too far into the future");
  srsran_sanity_check(pucch_cfg.sr_res_list.size() == 1, "UE SR resource list must have size 1.");

  auto& slot_record = get_slot_resource_counter(slot_sr);

  // We assume each UE only has 1 SR Resource Config configured.
  const unsigned sr_pucch_res_id = pucch_cfg.sr_res_list[0].pucch_res_id.cell_res_id;
  // Check if the wanted resource is already allocated to another UE in this slot.
  if (sr_pucch_res_id >= slot_record.ues_using_pucch_res.size() or
      (slot_record.ues_using_pucch_res[sr_pucch_res_id].rnti != rnti_t::INVALID_RNTI and
       slot_record.ues_using_pucch_res[sr_pucch_res_id].rnti != crnti)) {
    return nullptr;
  }

  const auto& pucch_res_list = pucch_cfg.pucch_res_list;
  // Search for the PUCCH resource with the correct PUCCH resource ID from the PUCCH resource list.
  const auto* res_cfg =
      std::find_if(pucch_res_list.begin(), pucch_res_list.end(), [sr_pucch_res_id](const pucch_resource& res) {
        return res.res_id.cell_res_id == sr_pucch_res_id;
      });

  if (res_cfg == pucch_res_list.end()) {
    return nullptr;
  }

  // If the PUCCH res with correct ID was not allocated to the UE's RNTI, allocate it to this RNTI; otherwise, it means
  // the resource had already been allocated, just return it.
  if (slot_record.ues_using_pucch_res[sr_pucch_res_id].rnti != crnti) {
    slot_record.ues_using_pucch_res[sr_pucch_res_id].rnti           = crnti;
    slot_record.ues_using_pucch_res[sr_pucch_res_id].resource_usage = pucch_resource_usage::SR;
  }

  return res_cfg;
}

bool pucch_resource_manager::release_harq_set_0_resource(slot_point          slot_harq,
                                                         rnti_t              crnti,
                                                         const pucch_config& pucch_cfg)
{
  return release_harq_resource(slot_harq, crnti, pucch_cfg, pucch_res_set_idx::set_0);
}

bool pucch_resource_manager::release_harq_set_1_resource(slot_point          slot_harq,
                                                         rnti_t              crnti,
                                                         const pucch_config& pucch_cfg)
{
  return release_harq_resource(slot_harq, crnti, pucch_cfg, pucch_res_set_idx::set_1);
}

bool pucch_resource_manager::release_sr_resource(slot_point slot_sr, rnti_t crnti, const pucch_config& pucch_cfg)
{
  srsran_sanity_check(slot_sr < last_sl_ind + RES_MANAGER_RING_BUFFER_SIZE,
                      "PUCCH being allocated too far into the future");
  srsran_sanity_check(pucch_cfg.sr_res_list.size() == 1, "UE SR resource list must have size 1.");

  auto& slot_record = get_slot_resource_counter(slot_sr);

  // We assume each UE only has 1 SR Resource Config configured.
  const unsigned sr_pucch_res_id = pucch_cfg.sr_res_list[0].pucch_res_id.cell_res_id;

  if (sr_pucch_res_id >= slot_record.ues_using_pucch_res.size() or
      slot_record.ues_using_pucch_res[sr_pucch_res_id].rnti != crnti) {
    return false;
  }

  slot_record.ues_using_pucch_res[sr_pucch_res_id].rnti           = rnti_t::INVALID_RNTI;
  slot_record.ues_using_pucch_res[sr_pucch_res_id].resource_usage = pucch_resource_usage::NOT_USED;
  return true;
}

bool pucch_resource_manager::release_csi_resource(slot_point                   slot_csi,
                                                  rnti_t                       crnti,
                                                  const ue_cell_configuration& ue_cell_cfg)
{
  srsran_sanity_check(slot_csi < last_sl_ind + RES_MANAGER_RING_BUFFER_SIZE,
                      "PUCCH being allocated too far into the future");

  auto& slot_record = get_slot_resource_counter(slot_csi);

  // We assume each UE only has 1 CSI Resource Config configured.
  const int csi_pucch_res_idx = get_pucch_res_idx_for_csi(ue_cell_cfg);
  if (csi_pucch_res_idx < 0) {
    return false;
  }
  if (static_cast<unsigned>(csi_pucch_res_idx) >= slot_record.ues_using_pucch_res.size() or
      slot_record.ues_using_pucch_res[csi_pucch_res_idx].rnti != crnti) {
    return false;
  }

  slot_record.ues_using_pucch_res[csi_pucch_res_idx].rnti           = rnti_t::INVALID_RNTI;
  slot_record.ues_using_pucch_res[csi_pucch_res_idx].resource_usage = pucch_resource_usage::NOT_USED;
  return true;
}

void pucch_resource_manager::reset_latest_reserved_res_tracker()
{
  last_ue_allocations.rnti       = rnti_t::INVALID_RNTI;
  last_ue_allocations.harq_set_0 = false;
  last_ue_allocations.harq_set_1 = false;
  last_ue_allocations.sr         = false;
  last_ue_allocations.csi        = false;
}

void pucch_resource_manager::set_new_resource_allocation(rnti_t crnti, pucch_resource_usage res_type)
{
  if (last_ue_allocations.rnti != crnti and last_ue_allocations.rnti != rnti_t::INVALID_RNTI) {
    srsran_assertion_failure("The last UE allocation tracker was already in used by another RNTI");
    return;
  }

  last_ue_allocations.rnti = crnti;

  switch (res_type) {
    case pucch_resource_usage::HARQ_SET_0:
      last_ue_allocations.harq_set_0 = true;
      break;
    case pucch_resource_usage::HARQ_SET_1:
      last_ue_allocations.harq_set_1 = true;
      break;
    case pucch_resource_usage::SR:
      last_ue_allocations.sr = true;
      break;
    case pucch_resource_usage::CSI:
      last_ue_allocations.csi = true;
      break;
    default:
      srsran_assertion_failure("Invalid PUCCH resource usage type");
  }
}

bool pucch_resource_manager::is_resource_allocated(rnti_t rnti, pucch_resource_usage res_type) const
{
  if (last_ue_allocations.rnti != rnti and last_ue_allocations.rnti != rnti_t::INVALID_RNTI) {
    srsran_assertion_failure("The last UE allocation tracker was already in used by another RNTI");
    return false;
  }

  switch (res_type) {
    case pucch_resource_usage::HARQ_SET_0:
      return last_ue_allocations.harq_set_0;
    case pucch_resource_usage::HARQ_SET_1:
      return last_ue_allocations.harq_set_1;
    case pucch_resource_usage::SR:
      return last_ue_allocations.sr;
    case pucch_resource_usage::CSI:
      return last_ue_allocations.csi;
    default:
      srsran_assertion_failure("Invalid PUCCH resource usage type");
      return false;
  }
}

void pucch_resource_manager::cancel_last_ue_res_reservations(slot_point                   slot_tx,
                                                             rnti_t                       crnti,
                                                             const ue_cell_configuration& ue_cell_cfg)
{
  if (last_ue_allocations.rnti == rnti_t::INVALID_RNTI) {
    return;
  }

  if (crnti != last_ue_allocations.rnti) {
    srsran_assertion_failure("rnti={}: cancelling PUCCH resource reservations of another UE (rnti={}) is not allowed",
                             crnti,
                             last_ue_allocations.rnti);
    return;
  }

  if (last_ue_allocations.harq_set_0) {
    release_harq_set_0_resource(slot_tx, crnti, ue_cell_cfg.init_bwp().ul_ded->pucch_cfg.value());
  }
  if (last_ue_allocations.harq_set_1) {
    release_harq_set_1_resource(slot_tx, crnti, ue_cell_cfg.init_bwp().ul_ded->pucch_cfg.value());
  }
  if (last_ue_allocations.sr) {
    release_sr_resource(slot_tx, crnti, ue_cell_cfg.init_bwp().ul_ded->pucch_cfg.value());
  }
  if (last_ue_allocations.csi) {
    release_csi_resource(slot_tx, crnti, ue_cell_cfg);
  }
}

pucch_harq_resource_alloc_record pucch_resource_manager::reserve_next_harq_res_available(slot_point          slot_harq,
                                                                                         rnti_t              crnti,
                                                                                         const pucch_config& pucch_cfg,
                                                                                         pucch_res_set_idx res_set_idx)
{
  srsran_sanity_check(slot_harq < last_sl_ind + RES_MANAGER_RING_BUFFER_SIZE,
                      "PUCCH being allocated too far into the future");

  // Get resource list of wanted slot.
  rnti_pucch_res_id_slot_record& res_counter = get_slot_resource_counter(slot_harq);

  auto& slot_res_array = res_counter.ues_using_pucch_res;

  // Get the span over the array of resources for the specific UE.
  const auto& ue_res_id_set_for_harq =
      pucch_cfg.pucch_res_set[pucch_res_set_idx_to_uint(res_set_idx)].pucch_res_id_list;
  unsigned ue_first_res_id = ue_res_id_set_for_harq.front().cell_res_id;

  // [Implementation-defined] Format 0/1 resources are at the beginning of the list, while Format 2/3/4 are at the end.
  const bool is_format0 = pucch_cfg.pucch_res_list.front().format == pucch_format::FORMAT_0;
  const bool is_format2 = pucch_cfg.pucch_res_list.back().format == pucch_format::FORMAT_2;

  srsran_assert(ue_first_res_id + ue_res_id_set_for_harq.size() <= slot_res_array.size(),
                "Indexing of PUCCH resource set exceeds the size of the cell resource array");
  // For PUCCH format 0 and format 2, we don't use the last 2 resources of the PUCCH resource set; these are reserved
  // the UE for CSI and SR slots and should only be picked for through the specific PUCCH resource indicator.
  const unsigned nof_eligible_resource =
      is_format0 and is_format2 ? ue_res_id_set_for_harq.size() - 2U : ue_res_id_set_for_harq.size();
  srsran_assert(nof_eligible_resource >= 1U,
                "rnti={}: Not enough eligible resources from PUCCH resource set={}",
                crnti,
                pucch_res_set_idx_to_uint(res_set_idx));
  span<resource_tracker> slot_ue_res_array(&slot_res_array[ue_first_res_id], nof_eligible_resource);

  // Check first if there is any PUCCH resource already used by this UE.
  auto* available_resource = std::find_if(slot_ue_res_array.begin(),
                                          slot_ue_res_array.end(),
                                          [crnti](const resource_tracker res) { return res.rnti == crnti; });

  // If not, find an available resource.
  if (available_resource == slot_ue_res_array.end()) {
    available_resource = std::find_if(slot_ue_res_array.begin(),
                                      slot_ue_res_array.end(),
                                      [](const resource_tracker res) { return res.rnti == rnti_t::INVALID_RNTI; });
  }

  const auto& pucch_res_list = pucch_cfg.pucch_res_list;

  // If there is an available resource, try to allocate it.
  if (available_resource != slot_ue_res_array.end() and
      static_cast<unsigned>(available_resource - slot_ue_res_array.begin()) < nof_eligible_resource) {
    // Get the PUCCH resource indicator from the available resource position within the span.
    const auto pucch_res_indicator = static_cast<unsigned>(available_resource - slot_ue_res_array.begin());
    // Get the PUCCH resource ID from the PUCCH resource indicator and the PUCCH resource set.
    const unsigned pucch_res_idx_from_list = ue_res_id_set_for_harq[pucch_res_indicator].cell_res_id;

    // Search for the PUCCH resource with the correct PUCCH resource ID from the PUCCH resource list.
    const auto* res_cfg = std::find_if(
        pucch_res_list.begin(), pucch_res_list.end(), [pucch_res_idx_from_list](const pucch_resource& res) {
          return res.res_id.cell_res_id == pucch_res_idx_from_list;
        });

    // If so, allocate it.
    if (res_cfg != pucch_res_list.end()) {
      available_resource->rnti = crnti;
      available_resource->resource_usage =
          res_set_idx == pucch_res_set_idx::set_0 ? pucch_resource_usage::HARQ_SET_0 : pucch_resource_usage::HARQ_SET_1;
      return pucch_harq_resource_alloc_record{.pucch_res = res_cfg, .pucch_res_indicator = pucch_res_indicator};
    }
  }
  return pucch_harq_resource_alloc_record{.pucch_res = nullptr};
}

const pucch_resource* pucch_resource_manager::reserve_harq_res_by_res_indicator(slot_point          slot_harq,
                                                                                rnti_t              crnti,
                                                                                unsigned            res_indicator,
                                                                                const pucch_config& pucch_cfg,
                                                                                pucch_res_set_idx   res_set_idx)
{
  srsran_sanity_check(slot_harq < last_sl_ind + RES_MANAGER_RING_BUFFER_SIZE,
                      "PUCCH being allocated too far into the future");

  // Get resource list of wanted slot.
  rnti_pucch_res_id_slot_record& res_counter = get_slot_resource_counter(slot_harq);
  // Retrieve the PUCCH resource set.
  const auto& ue_res_id_set_for_harq =
      pucch_cfg.pucch_res_set[pucch_res_set_idx_to_uint(res_set_idx)].pucch_res_id_list;

  // Make sure the resource indicator points to a valid resource.
  if (res_indicator >= ue_res_id_set_for_harq.size()) {
    return nullptr;
  }

  // Get PUCCH resource ID from the PUCCH resource set.
  const auto pucch_res_id = ue_res_id_set_for_harq[res_indicator];

  // [Implementation-defined] Format 0/1 resources are at the beginning of the list, while Format 2/3/4 are at the end.
  const bool is_format0 = pucch_cfg.pucch_res_list.front().format == pucch_format::FORMAT_0;
  const bool is_format2 = pucch_cfg.pucch_res_list.back().format == pucch_format::FORMAT_2;

  // For Format 0 and Format 2, the resources indexed by PUCCH res. indicators >= ue_res_id_set_for_harq.size() - 2 are
  // reserved for CSI and SR slots. In the case, we don't need to reserve these in the PUCCH resource manager, we only
  // need to return the resouces.
  if (is_format0 and is_format2 and res_indicator >= ue_res_id_set_for_harq.size() - 2U) {
    const auto* res_cfg = std::find_if(
        pucch_cfg.pucch_res_list.begin(), pucch_cfg.pucch_res_list.end(), [pucch_res_id](const pucch_resource& res) {
          return res.res_id.ue_res_id == pucch_res_id.ue_res_id;
        });
    return res_cfg != pucch_cfg.pucch_res_list.end() ? &(*res_cfg) : nullptr;
  }

  // Get PUCCH resource ID from the PUCCH resource set.
  const unsigned pucch_cell_res_id = pucch_res_id.cell_res_id;

  // Extra safe check. Make sure cell_res_id is valid, even though this should be ensured by the UE config validator.
  if (pucch_cell_res_id >= res_counter.ues_using_pucch_res.size()) {
    return nullptr;
  }

  // Get the PUCCH resource tracker in the PUCCH resource manager.
  auto&       pucch_res_tracker = res_counter.ues_using_pucch_res[pucch_cell_res_id];
  const auto& pucch_res_list    = pucch_cfg.pucch_res_list;

  // Check first if the wanted PUCCH resource is available.
  if (pucch_res_tracker.rnti != rnti_t::INVALID_RNTI and pucch_res_tracker.rnti != crnti) {
    return nullptr;
  }

  // Search for the PUCCH resource with the correct PUCCH resource ID from the PUCCH resource list.
  const auto* res_cfg =
      std::find_if(pucch_res_list.begin(), pucch_res_list.end(), [pucch_cell_res_id](const pucch_resource& res) {
        return res.res_id.cell_res_id == pucch_cell_res_id;
      });

  if (res_cfg == pucch_res_list.end()) {
    return nullptr;
  }

  // If the PUCCH res with correct ID is found and previously not used by given UE's RNTI, then allocate it this RNTI.
  if (pucch_res_tracker.rnti == rnti_t::INVALID_RNTI) {
    pucch_res_tracker.rnti = crnti;
    pucch_res_tracker.resource_usage =
        res_set_idx == pucch_res_set_idx::set_0 ? pucch_resource_usage::HARQ_SET_0 : pucch_resource_usage::HARQ_SET_1;
  }
  return res_cfg;
}

bool pucch_resource_manager::release_harq_resource(slot_point          slot_harq,
                                                   rnti_t              crnti,
                                                   const pucch_config& pucch_cfg,
                                                   pucch_res_set_idx   res_set_idx)
{
  srsran_sanity_check(slot_harq < last_sl_ind + RES_MANAGER_RING_BUFFER_SIZE,
                      "PUCCH being allocated too far into the future");

  // Get resource list of wanted slot.
  rnti_pucch_res_id_slot_record& res_counter = get_slot_resource_counter(slot_harq);

  auto& slot_res_array = res_counter.ues_using_pucch_res;

  const pucch_resource_usage res_usage =
      res_set_idx == pucch_res_set_idx::set_0 ? pucch_resource_usage::HARQ_SET_0 : pucch_resource_usage::HARQ_SET_1;

  // [Implementation-defined] Format 0/1 resources are at the beginning of the list, while Format 2/3/4 are at the end.
  const bool is_format0 = pucch_cfg.pucch_res_list.front().format == pucch_format::FORMAT_0;
  const bool is_format2 = pucch_cfg.pucch_res_list.back().format == pucch_format::FORMAT_2;

  // Get the span over the array of resources for the specific UE.
  const auto& ue_res_id_set_for_harq =
      pucch_cfg.pucch_res_set[pucch_res_set_idx_to_uint(res_set_idx)].pucch_res_id_list;
  unsigned ue_first_res_id = ue_res_id_set_for_harq.front().cell_res_id;
  srsran_assert(ue_first_res_id + ue_res_id_set_for_harq.size() <= slot_res_array.size(),
                "Indexing of PUCCH resource set exceeds the size of the cell resource array");
  // For PUCCH format 0 and format 2, we don't use the last 2 resources of the PUCCH resource set; these are reserved
  // the UE for CSI and SR slots and should only be picked for through the specific PUCCH resource indicator.
  const unsigned nof_eligible_resource =
      is_format0 and is_format2 ? ue_res_id_set_for_harq.size() - 2U : ue_res_id_set_for_harq.size();
  srsran_assert(nof_eligible_resource >= 1U,
                "rnti={}: Not enough eligible resources from PUCCH resource set={}",
                crnti,
                pucch_res_set_idx_to_uint(res_set_idx));
  span<resource_tracker> slot_ue_res_array(&slot_res_array[ue_first_res_id], nof_eligible_resource);

  // Check first if the target PUCCH resource (given the CRNTI and usage) exists within the resource tracker.
  auto* target_res =
      std::find_if(slot_ue_res_array.begin(), slot_ue_res_array.end(), [crnti, res_usage](const resource_tracker& res) {
        return res.rnti == crnti and res.resource_usage == res_usage;
      });

  // If the resources was found, then release it (i.e., remove the C-RNTI of the user allocated to it).
  if (target_res != slot_ue_res_array.end()) {
    target_res->rnti           = rnti_t::INVALID_RNTI;
    target_res->resource_usage = pucch_resource_usage::NOT_USED;
    return true;
  }

  return false;
}

pucch_resource_manager::rnti_pucch_res_id_slot_record& pucch_resource_manager::get_slot_resource_counter(slot_point sl)
{
  srsran_sanity_check(sl < last_sl_ind + RES_MANAGER_RING_BUFFER_SIZE,
                      "PUCCH resource ring-buffer accessed too far into the future");
  return resource_slots[sl.to_uint() % RES_MANAGER_RING_BUFFER_SIZE];
}
