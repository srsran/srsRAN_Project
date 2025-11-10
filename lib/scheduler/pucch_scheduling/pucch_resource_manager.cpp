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

#include "pucch_resource_manager.h"
#include "pucch_collision_manager.h"
#include "srsran/adt/static_vector.h"
#include "srsran/ran/rnti.h"
#include "srsran/support/srsran_assert.h"

using namespace srsran;

/////////////    RESOURCE MANAGER     /////////////

/////////////   Static function   /////////////

static int get_csi_cell_res_id(const ue_cell_configuration& ue_cell_cfg)
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

pucch_resource_manager::pucch_resource_manager(const cell_configuration& cell_cfg_) :
  cell_cfg(cell_cfg_),
  collision_manager(cell_cfg_),
  slots_ctx({static_vector<rnti_t, pucch_constants::MAX_NOF_CELL_PUCCH_RESOURCES>(cell_cfg_.ded_pucch_resources.size(),
                                                                                  rnti_t::INVALID_RNTI)})
{
}

void pucch_resource_manager::slot_indication(slot_point slot_tx)
{
  // If last_sl_ind is not valid (not initialized), then the check sl_tx == last_sl_ind + 1 does not matter.
  srsran_sanity_check(not last_sl_ind.valid() or slot_tx == last_sl_ind + 1, "Detected a skipped slot");

  // Update Slot.
  last_sl_ind = slot_tx;

  // Clear previous slot context.
  auto& ctx = slots_ctx[slot_tx.to_uint() - 1];
  std::fill(ctx.ues_using_pucch_res.begin(), ctx.ues_using_pucch_res.end(), rnti_t::INVALID_RNTI);

  collision_manager.slot_indication(slot_tx);
}

void pucch_resource_manager::stop()
{
  for (auto& ctx : slots_ctx) {
    std::fill(ctx.ues_using_pucch_res.begin(), ctx.ues_using_pucch_res.end(), rnti_t::INVALID_RNTI);
  }
  last_sl_ind = {};
  collision_manager.stop();
}

bool pucch_resource_manager::is_harq_common_resource_available(slot_point sl, size_t r_pucch)
{
  srsran_assert(r_pucch < 16, "r_PUCCH must be less than 16");
  // TODO: Remove this method and use collision_manager.alloc_common directly once it checks for UL grant collisions
  // correctly.
  auto res = collision_manager.alloc_common(sl, r_pucch);
  if (res.has_value()) {
    // Allocation was successful, free the resource and return true.
    collision_manager.free_common(sl, r_pucch);
    return true;
  }
  return false;
}

void pucch_resource_manager::reserve_harq_common_resource(slot_point sl, size_t r_pucch)
{
  srsran_assert(r_pucch < 16, "r_PUCCH must be less than 16");
  collision_manager.alloc_common(sl, r_pucch);
}

/////////////   UE Reservation Guard   /////////////

pucch_resource_manager::ue_reservation_guard::ue_reservation_guard(pucch_resource_manager*      parent_,
                                                                   rnti_t                       rnti_,
                                                                   slot_point                   sl_,
                                                                   const ue_cell_configuration& ue_cfg_) :
  parent(parent_), rnti(rnti_), sl(sl_), ue_cfg(ue_cfg_)
{
  srsran_sanity_check(parent != nullptr, "PUCCH Resource Manager pointer cannot be null");
  srsran_sanity_check(sl < parent->last_sl_ind + RES_MANAGER_RING_BUFFER_SIZE,
                      "PUCCH being allocated too far into the future");
  srsran_sanity_check(rnti != rnti_t::INVALID_RNTI, "RNTI cannot be invalid");
}

pucch_resource_manager::ue_reservation_guard::~ue_reservation_guard()
{
  rollback();
}

pucch_harq_resource_alloc_record
pucch_resource_manager::ue_reservation_guard::reserve_harq_set_0_resource_next_available()
{
  return reserve_next_harq_res_available(pucch_res_set_idx::set_0);
}

pucch_harq_resource_alloc_record
pucch_resource_manager::ue_reservation_guard::reserve_harq_set_1_resource_next_available()
{
  return reserve_next_harq_res_available(pucch_res_set_idx::set_1);
}

const pucch_resource*
pucch_resource_manager::ue_reservation_guard::reserve_harq_set_0_resource_by_res_indicator(unsigned d_pri)

{
  return reserve_harq_resource_by_res_indicator(d_pri, pucch_res_set_idx::set_0);
}

const pucch_resource*
pucch_resource_manager::ue_reservation_guard::reserve_harq_set_1_resource_by_res_indicator(unsigned d_pri)
{
  return reserve_harq_resource_by_res_indicator(d_pri, pucch_res_set_idx::set_1);
}

const pucch_resource* pucch_resource_manager::ue_reservation_guard::reserve_sr_resource()
{
  srsran_assert(parent != nullptr, "Trying to make a new PUCCH resource reservation after commit has been called");
  const auto& pucch_cfg = ue_cfg.init_bwp().ul_ded->pucch_cfg.value();

  // We assume each UE only has 1 SR Resource Config configured.
  srsran_sanity_check(pucch_cfg.sr_res_list.size() == 1, "UE SR resource list must have size 1.");
  const unsigned cell_res_id = pucch_cfg.sr_res_list[0].pucch_res_id.cell_res_id;

  // Get resource list of wanted slot.
  auto& ctx = parent->slots_ctx[sl.to_uint()];
  srsran_assert(cell_res_id < ctx.ues_using_pucch_res.size(),
                "rnti={}: CSI PUCCH resource index exceeds the size of the cell resource array",
                rnti);

  // Check if the wanted resource is already allocated to another UE in this slot.
  auto& res_rnti = ctx.ues_using_pucch_res[cell_res_id];
  if (res_rnti != rnti_t::INVALID_RNTI and res_rnti != rnti) {
    return nullptr;
  }

  // Search for the PUCCH resource with the correct PUCCH resource ID from the PUCCH resource list.
  const auto& pucch_res_list = pucch_cfg.pucch_res_list;
  const auto* res_cfg =
      std::find_if(pucch_res_list.begin(), pucch_res_list.end(), [cell_res_id](const pucch_resource& res) {
        return res.res_id.cell_res_id == cell_res_id;
      });
  srsran_assert(res_cfg != pucch_res_list.end(),
                "rnti={}: PUCCH resource with cell_res_id={} not found in PUCCH resource list",
                rnti,
                cell_res_id);

  // If the PUCCH res with correct ID was not allocated to the UE's RNTI, allocate it to this RNTI; otherwise, it means
  // the resource had already been allocated, just return it.
  if (res_rnti != rnti) {
    // Check for collisions.
    if (not parent->collision_manager.alloc_ded(sl, cell_res_id).has_value()) {
      return nullptr;
    }
    res_rnti                                                     = rnti;
    reservations[static_cast<unsigned>(resource_usage_type::sr)] = {cell_res_id};
  }

  return res_cfg;
}

const pucch_resource* pucch_resource_manager::ue_reservation_guard::reserve_csi_resource()
{
  srsran_assert(parent != nullptr, "Trying to make a new PUCCH resource reservation after commit has been called");

  // Get CSI specific PUCCH resource ID from the CSI meas config.
  const int cell_res_id_int = get_csi_cell_res_id(ue_cfg);
  srsran_assert(cell_res_id_int >= 0, "rnti={}: CSI PUCCH resource index could not be found", rnti);
  const auto cell_res_id = static_cast<unsigned>(cell_res_id_int);

  // Get resource list of wanted slot.
  auto& ctx = parent->slots_ctx[sl.to_uint()];
  srsran_assert(cell_res_id < ctx.ues_using_pucch_res.size(),
                "rnti={}: CSI PUCCH resource index exceeds the size of the cell resource array",
                rnti);

  // Check if the wanted resource is already allocated to another UE in this slot.
  auto& res_rnti = ctx.ues_using_pucch_res[cell_res_id];
  if (res_rnti != rnti_t::INVALID_RNTI and res_rnti != rnti) {
    return nullptr;
  }

  // Search for the PUCCH resource with the correct PUCCH resource ID from the PUCCH resource list.
  const auto& pucch_res_list = ue_cfg.init_bwp().ul_ded->pucch_cfg.value().pucch_res_list;
  const auto* res_cfg =
      std::find_if(pucch_res_list.begin(), pucch_res_list.end(), [cell_res_id](const pucch_resource& res) {
        return res.res_id.cell_res_id == cell_res_id;
      });
  srsran_assert(res_cfg != pucch_res_list.end(),
                "rnti={}: PUCCH resource with cell_res_id={} not found in PUCCH resource list",
                rnti,
                cell_res_id);

  // If the PUCCH res with correct ID was not allocated to the UE's RNTI, allocate it to this RNTI; otherwise, it means
  // the resource had already been allocated, just return it.
  if (res_rnti != rnti) {
    // Check for collisions.
    if (not parent->collision_manager.alloc_ded(sl, cell_res_id).has_value()) {
      return nullptr;
    }
    res_rnti                                                      = rnti;
    reservations[static_cast<unsigned>(resource_usage_type::csi)] = {cell_res_id};
  }

  return res_cfg;
}

const pucch_resource* pucch_resource_manager::ue_reservation_guard::peek_sr_resource() const
{
  srsran_assert(parent != nullptr, "Trying to make a new PUCCH resource reservation after commit has been called");
  const auto& pucch_cfg = ue_cfg.init_bwp().ul_ded->pucch_cfg.value();

  // We assume each UE only has 1 SR Resource Config configured.
  srsran_sanity_check(pucch_cfg.sr_res_list.size() == 1, "UE SR resource list must have size 1.");
  const unsigned cell_res_id = pucch_cfg.sr_res_list[0].pucch_res_id.cell_res_id;

  // Search for the PUCCH resource with the correct PUCCH resource ID from the PUCCH resource list.
  const auto& pucch_res_list = pucch_cfg.pucch_res_list;
  const auto* res_cfg =
      std::find_if(pucch_res_list.begin(), pucch_res_list.end(), [cell_res_id](const pucch_resource& res) {
        return res.res_id.cell_res_id == cell_res_id;
      });
  srsran_assert(res_cfg != pucch_res_list.end(),
                "rnti={}: PUCCH resource with cell_res_id={} not found in PUCCH resource list",
                rnti,
                cell_res_id);

  return res_cfg;
}

const pucch_resource* pucch_resource_manager::ue_reservation_guard::peek_csi_resource() const
{
  srsran_assert(parent != nullptr, "Trying to make a new PUCCH resource reservation after commit has been called");

  // Get CSI specific PUCCH resource ID from the CSI meas config.
  const int cell_res_id_int = get_csi_cell_res_id(ue_cfg);
  srsran_assert(cell_res_id_int >= 0, "rnti={}: CSI PUCCH resource index could not be found", rnti);
  const auto cell_res_id = static_cast<unsigned>(cell_res_id_int);

  // Search for the PUCCH resource with the correct PUCCH resource ID from the PUCCH resource list.
  const auto& pucch_res_list = ue_cfg.init_bwp().ul_ded->pucch_cfg.value().pucch_res_list;
  const auto* res_cfg =
      std::find_if(pucch_res_list.begin(), pucch_res_list.end(), [cell_res_id](const pucch_resource& res) {
        return res.res_id.cell_res_id == cell_res_id;
      });
  srsran_assert(res_cfg != pucch_res_list.end(),
                "rnti={}: PUCCH resource with cell_res_id={} not found in PUCCH resource list",
                rnti,
                cell_res_id);

  return res_cfg;
}

bool pucch_resource_manager::ue_reservation_guard::release_harq_set_0_resource()
{
  return release_harq_resource(pucch_res_set_idx::set_0);
}

bool pucch_resource_manager::ue_reservation_guard::release_harq_set_1_resource()
{
  return release_harq_resource(pucch_res_set_idx::set_1);
}

bool pucch_resource_manager::ue_reservation_guard::release_sr_resource()
{
  srsran_assert(parent != nullptr, "Trying to release a PUCCH resource after commit has been called");
  const auto& pucch_cfg = ue_cfg.init_bwp().ul_ded->pucch_cfg.value();
  srsran_sanity_check(pucch_cfg.sr_res_list.size() == 1, "UE SR resource list must have size 1.");

  // We assume each UE only has 1 SR Resource Config configured.
  const unsigned cell_res_id = pucch_cfg.sr_res_list[0].pucch_res_id.cell_res_id;

  // Get resource list of wanted slot.
  auto& ctx = parent->slots_ctx[sl.to_uint()];
  srsran_assert(cell_res_id < ctx.ues_using_pucch_res.size(),
                "rnti={}: SR PUCCH resource index exceeds the size of the cell resource array",
                rnti);

  // Check if the resource is allocated to this RNTI.
  if (ctx.ues_using_pucch_res[cell_res_id] != rnti) {
    return false;
  }

  // Release the resource.
  ctx.ues_using_pucch_res[cell_res_id] = rnti_t::INVALID_RNTI;
  parent->collision_manager.free_ded(sl, cell_res_id);
  reservations[static_cast<unsigned>(resource_usage_type::sr)].cell_res_id = std::nullopt;
  return true;
}

bool pucch_resource_manager::ue_reservation_guard::release_csi_resource()
{
  srsran_assert(parent != nullptr, "Trying to release a PUCCH resource after commit has been called");

  // Get CSI specific PUCCH resource ID from the CSI meas config.
  const int cell_res_id_int = get_csi_cell_res_id(ue_cfg);
  srsran_assert(cell_res_id_int >= 0, "rnti={}: CSI PUCCH resource index could not be found", rnti);
  const auto cell_res_id = static_cast<unsigned>(cell_res_id_int);

  // Get resource list of wanted slot.
  auto& ctx = parent->slots_ctx[sl.to_uint()];
  srsran_assert(cell_res_id < ctx.ues_using_pucch_res.size(),
                "rnti={}: SR PUCCH resource index exceeds the size of the cell resource array",
                rnti);

  // Check if the resource is allocated to this RNTI.
  if (ctx.ues_using_pucch_res[cell_res_id] != rnti) {
    return false;
  }

  // Release the resource.
  ctx.ues_using_pucch_res[cell_res_id] = rnti_t::INVALID_RNTI;
  parent->collision_manager.free_ded(sl, cell_res_id);
  reservations[static_cast<unsigned>(resource_usage_type::csi)].cell_res_id = std::nullopt;
  return true;
}

pucch_harq_resource_alloc_record
pucch_resource_manager::ue_reservation_guard::reserve_next_harq_res_available(pucch_res_set_idx res_set_idx)
{
  srsran_assert(parent != nullptr, "Trying to make a new PUCCH resource reservation after commit has been called");
  const auto& pucch_cfg = ue_cfg.init_bwp().ul_ded->pucch_cfg.value();

  // Get the array of resources for the specific UE.
  const auto& ue_res_set_id_list = pucch_cfg.pucch_res_set[pucch_res_set_idx_to_uint(res_set_idx)].pucch_res_id_list;

  // For PUCCH F0 and F2, we don't use the last 2 resources of the PUCCH resource set; these are reserved for CSI and SR
  // slots and should only be picked for through the specific PUCCH resource indicator.
  const unsigned nof_eligible_resources =
      parent->cell_cfg.is_pucch_f0_and_f2() ? ue_res_set_id_list.size() - 2U : ue_res_set_id_list.size();
  srsran_assert(nof_eligible_resources >= 1U,
                "rnti={}: Not enough eligible resources from PUCCH resource set={}",
                rnti,
                pucch_res_set_idx_to_uint(res_set_idx));

  // Get context of wanted slot.
  auto& ctx = parent->slots_ctx[sl.to_uint()];

  // If a resource is already allocated to this RNTI, use it.
  std::optional<unsigned> available_res;
  for (uint8_t r_pucch = 0; r_pucch != nof_eligible_resources; ++r_pucch) {
    const unsigned cell_res_id = ue_res_set_id_list[r_pucch].cell_res_id;
    srsran_assert(cell_res_id < ctx.ues_using_pucch_res.size(),
                  "PUCCH resource index from PUCCH resource set exceeds the size of the cell resource array");
    auto& res_rnti = ctx.ues_using_pucch_res[cell_res_id];

    if (res_rnti == rnti) {
      available_res = r_pucch;
      break;
    }
  }

  if (not available_res.has_value()) {
    // Else, try to allocate the first available resource.
    for (unsigned r_pucch = 0; r_pucch != nof_eligible_resources; ++r_pucch) {
      const unsigned cell_res_id = ue_res_set_id_list[r_pucch].cell_res_id;
      srsran_assert(cell_res_id < ctx.ues_using_pucch_res.size(),
                    "PUCCH resource index from PUCCH resource set exceeds the size of the cell resource array");
      auto& res_rnti = ctx.ues_using_pucch_res[cell_res_id];

      if (res_rnti == rnti_t::INVALID_RNTI and parent->collision_manager.alloc_ded(sl, cell_res_id).has_value()) {
        ctx.ues_using_pucch_res[cell_res_id] = rnti;
        unsigned usage_type_idx              = res_set_idx == pucch_res_set_idx::set_0
                                                   ? static_cast<unsigned>(resource_usage_type::harq_set_0)
                                                   : static_cast<unsigned>(resource_usage_type::harq_set_1);
        reservations[usage_type_idx]         = {cell_res_id};
        available_res                        = r_pucch;
        break;
      }
    }
  }

  // If an available resource was found, return it.
  if (available_res.has_value()) {
    const unsigned cell_res_id = ue_res_set_id_list[available_res.value()].cell_res_id;
    // Search for the PUCCH resource with the correct PUCCH resource ID from the PUCCH resource list.
    const auto& pucch_res_list = pucch_cfg.pucch_res_list;
    const auto* res_cfg =
        std::find_if(pucch_res_list.begin(), pucch_res_list.end(), [cell_res_id](const pucch_resource& res) {
          return res.res_id.cell_res_id == cell_res_id;
        });
    srsran_sanity_check(res_cfg != pucch_res_list.end(),
                        "rnti={}: PUCCH resource with cell_res_id={} not found in PUCCH resource list",
                        rnti,
                        cell_res_id);

    return pucch_harq_resource_alloc_record{.resource            = res_cfg,
                                            .pucch_res_indicator = static_cast<uint8_t>(available_res.value())};
  }
  return pucch_harq_resource_alloc_record{.resource = nullptr};
}

const pucch_resource*
pucch_resource_manager::ue_reservation_guard::reserve_harq_resource_by_res_indicator(unsigned          d_pri,
                                                                                     pucch_res_set_idx res_set_idx)
{
  srsran_assert(parent != nullptr, "Trying to make a new PUCCH resource reservation after commit has been called");
  const auto& pucch_cfg = ue_cfg.init_bwp().ul_ded->pucch_cfg.value();

  // Retrieve the PUCCH resource set.
  const auto& ue_res_set_id_list = pucch_cfg.pucch_res_set[pucch_res_set_idx_to_uint(res_set_idx)].pucch_res_id_list;

  // Get resource list of wanted slot.
  slot_context& ctx = parent->slots_ctx[sl.to_uint()];

  // Make sure the resource indicator points to a valid resource.
  if (d_pri >= ue_res_set_id_list.size()) {
    return nullptr;
  }

  // Get PUCCH resource ID from the PUCCH resource set.
  // [Implementation-defined] We assume at most 8 resources per resource set. If this is the case, r_pucch = d_pri.
  const auto pucch_res_id = ue_res_set_id_list[d_pri];

  // For Format 0 and Format 2, the resources indexed by PUCCH res. indicators >= ue_res_id_set_for_harq.size() - 2 are
  // reserved for CSI and SR slots. In the case, we don't need to reserve these in the PUCCH resource manager, we only
  // need to return the resources.
  if (parent->cell_cfg.is_pucch_f0_and_f2() and d_pri >= ue_res_set_id_list.size() - 2U) {
    const auto* res_cfg = std::find_if(
        pucch_cfg.pucch_res_list.begin(), pucch_cfg.pucch_res_list.end(), [pucch_res_id](const pucch_resource& res) {
          return res.res_id.ue_res_id == pucch_res_id.ue_res_id;
        });
    return res_cfg != pucch_cfg.pucch_res_list.end() ? &(*res_cfg) : nullptr;
  }

  // Get PUCCH resource ID from the PUCCH resource set.
  const unsigned cell_res_id = pucch_res_id.cell_res_id;
  srsran_assert(cell_res_id < ctx.ues_using_pucch_res.size(),
                "rnti={}: PUCCH resource index from PUCCH resource set exceeds the size of the cell resource array",
                rnti);

  // Check first if the wanted PUCCH resource is available.
  auto& res_rnti = ctx.ues_using_pucch_res[cell_res_id];
  if (res_rnti != rnti_t::INVALID_RNTI and res_rnti != rnti) {
    return nullptr;
  }

  // Search for the PUCCH resource with the correct PUCCH resource ID from the PUCCH resource list.
  const auto& pucch_res_list = pucch_cfg.pucch_res_list;
  const auto* res_cfg =
      std::find_if(pucch_res_list.begin(), pucch_res_list.end(), [cell_res_id](const pucch_resource& res) {
        return res.res_id.cell_res_id == cell_res_id;
      });
  srsran_assert(res_cfg != pucch_res_list.end(),
                "rnti={}: PUCCH resource with cell_res_id={} not found in PUCCH resource list",
                rnti,
                cell_res_id);

  if (res_rnti == rnti) {
    // If the resource is already allocated to this RNTI, just return it.
    return res_cfg;
  }

  // Allocate the resource to this RNTI.
  if (not parent->collision_manager.alloc_ded(sl, cell_res_id).has_value()) {
    return nullptr;
  }
  res_rnti                     = rnti;
  unsigned usage_type_idx      = res_set_idx == pucch_res_set_idx::set_0
                                     ? static_cast<unsigned>(resource_usage_type::harq_set_0)
                                     : static_cast<unsigned>(resource_usage_type::harq_set_1);
  reservations[usage_type_idx] = {cell_res_id};
  return res_cfg;
}

bool pucch_resource_manager::ue_reservation_guard::release_harq_resource(pucch_res_set_idx res_set_idx)
{
  srsran_assert(parent != nullptr, "Trying to release a PUCCH resource after commit has been called");
  // Get the array of resources for the specific UE.
  const auto& pucch_cfg          = ue_cfg.init_bwp().ul_ded->pucch_cfg.value();
  const auto& ue_res_set_id_list = pucch_cfg.pucch_res_set[pucch_res_set_idx_to_uint(res_set_idx)].pucch_res_id_list;

  // For PUCCH F0 and F2, we don't use the last 2 resources of the PUCCH resource set; these are reserved for CSI and SR
  // slots and should only be picked for through the specific PUCCH resource indicator.
  const unsigned nof_eligible_resource =
      parent->cell_cfg.is_pucch_f0_and_f2() ? ue_res_set_id_list.size() - 2U : ue_res_set_id_list.size();
  srsran_assert(nof_eligible_resource >= 1U,
                "rnti={}: Not enough eligible resources from PUCCH resource set={}",
                rnti,
                pucch_res_set_idx_to_uint(res_set_idx));

  // Get resource list of wanted slot.
  slot_context& ctx = parent->slots_ctx[sl.to_uint()];

  for (unsigned r_pucch = 0; r_pucch != nof_eligible_resource; ++r_pucch) {
    const unsigned cell_res_id = ue_res_set_id_list[r_pucch].cell_res_id;
    srsran_assert(cell_res_id < ctx.ues_using_pucch_res.size(),
                  "rnti={}: HARQ PUCCH resource index exceeds the size of the cell resource array");
    auto& res_rnti = ctx.ues_using_pucch_res[cell_res_id];

    if (res_rnti == rnti) {
      // Release the resource.
      res_rnti = rnti_t::INVALID_RNTI;
      parent->collision_manager.free_ded(sl, cell_res_id);
      unsigned usage_type_idx                  = res_set_idx == pucch_res_set_idx::set_0
                                                     ? static_cast<unsigned>(resource_usage_type::harq_set_0)
                                                     : static_cast<unsigned>(resource_usage_type::harq_set_1);
      reservations[usage_type_idx].cell_res_id = std::nullopt;
      return true;
    }
  }

  return false;
}

void pucch_resource_manager::ue_reservation_guard::commit()
{
  srsran_assert(parent != nullptr, "Trying to commit PUCCH resource reservations after commit has been called");

  // If both HARQ resource sets have been reserved, only keep the resource in PUCCH Resource Set ID 1.
  if (reservations[static_cast<unsigned>(resource_usage_type::harq_set_0)].cell_res_id.has_value() and
      reservations[static_cast<unsigned>(resource_usage_type::harq_set_1)].cell_res_id.has_value()) {
    // Release HARQ Resource Set ID 0 reservation.
    const unsigned cell_res_id =
        reservations[static_cast<unsigned>(resource_usage_type::harq_set_0)].cell_res_id.value();
    auto& ctx                            = parent->slots_ctx[sl.to_uint()];
    ctx.ues_using_pucch_res[cell_res_id] = rnti_t::INVALID_RNTI;
    parent->collision_manager.free_ded(sl, cell_res_id);
    reservations[static_cast<unsigned>(resource_usage_type::harq_set_0)].cell_res_id = std::nullopt;
  }

  // Clear parent pointer to avoid further reservations after commit.
  parent = nullptr;
}

void pucch_resource_manager::ue_reservation_guard::rollback()
{
  // If parent is nullptr, it means commit() was already called.
  if (parent != nullptr) {
    // Release all reservations made so far.
    for (auto& res : reservations) {
      if (res.cell_res_id.has_value()) {
        // Get resource list of wanted slot.
        auto& ctx = parent->slots_ctx[sl.to_uint()];
        srsran_assert(res.cell_res_id.value() < ctx.ues_using_pucch_res.size(),
                      "rnti={}: PUCCH resource index exceeds the size of the cell resource array",
                      rnti);

        // Release the resource.
        ctx.ues_using_pucch_res[res.cell_res_id.value()] = rnti_t::INVALID_RNTI;
        parent->collision_manager.free_ded(sl, res.cell_res_id.value());
      }
    }
  }
}
