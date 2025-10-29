/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pucch_resource_manager.h"
#include "pucch_collision_manager.h"
#include "srsran/adt/static_vector.h"

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
  return collision_manager.can_alloc_common(sl, r_pucch);
}

void pucch_resource_manager::reserve_harq_common_resource(slot_point sl, size_t r_pucch)
{
  srsran_assert(r_pucch < 16, "r_PUCCH must be less than 16");
  collision_manager.alloc_common(sl, r_pucch);
}

pucch_harq_resource_alloc_record
pucch_resource_manager::reserve_harq_set_0_resource_next_available(slot_point          slot_harq,
                                                                   rnti_t              crnti,
                                                                   const pucch_config& pucch_cfg)
{
  return reserve_next_harq_res_available(slot_harq, crnti, pucch_cfg, pucch_res_set_idx::set_0);
}

pucch_harq_resource_alloc_record
pucch_resource_manager::reserve_harq_set_1_resource_next_available(slot_point          slot_harq,
                                                                   rnti_t              crnti,
                                                                   const pucch_config& pucch_cfg)
{
  return reserve_next_harq_res_available(slot_harq, crnti, pucch_cfg, pucch_res_set_idx::set_1);
}

const pucch_resource*
pucch_resource_manager::reserve_harq_set_0_resource_by_res_indicator(slot_point          slot_harq,
                                                                     rnti_t              crnti,
                                                                     unsigned            d_pri,
                                                                     const pucch_config& pucch_cfg)
{
  return reserve_harq_resource_by_res_indicator(slot_harq, crnti, d_pri, pucch_cfg, pucch_res_set_idx::set_0);
}

const pucch_resource*
pucch_resource_manager::reserve_harq_set_1_resource_by_res_indicator(slot_point          slot_harq,
                                                                     rnti_t              crnti,
                                                                     unsigned            d_pri,
                                                                     const pucch_config& pucch_cfg)
{
  return reserve_harq_resource_by_res_indicator(slot_harq, crnti, d_pri, pucch_cfg, pucch_res_set_idx::set_1);
}

const pucch_resource*
pucch_resource_manager::reserve_sr_resource(slot_point slot_sr, rnti_t crnti, const pucch_config& pucch_cfg)
{
  srsran_sanity_check(slot_sr < last_sl_ind + RES_MANAGER_RING_BUFFER_SIZE,
                      "PUCCH being allocated too far into the future");
  srsran_sanity_check(pucch_cfg.sr_res_list.size() == 1, "UE SR resource list must have size 1.");

  // We assume each UE only has 1 SR Resource Config configured.
  const unsigned cell_res_id = pucch_cfg.sr_res_list[0].pucch_res_id.cell_res_id;

  // Get resource list of wanted slot.
  auto& ctx = slots_ctx[slot_sr.to_uint()];
  srsran_assert(cell_res_id < ctx.ues_using_pucch_res.size(),
                "rnti={}: CSI PUCCH resource index exceeds the size of the cell resource array",
                crnti);

  // Check if the wanted resource is already allocated to another UE in this slot.
  auto& res_rnti = ctx.ues_using_pucch_res[cell_res_id];
  if (res_rnti != rnti_t::INVALID_RNTI and res_rnti != crnti) {
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
                crnti,
                cell_res_id);

  // If the PUCCH res with correct ID was not allocated to the UE's RNTI, allocate it to this RNTI; otherwise, it means
  // the resource had already been allocated, just return it.
  if (res_rnti != crnti) {
    // Check for collisions.
    if (not collision_manager.can_alloc_ded(slot_sr, cell_res_id)) {
      return nullptr;
    }
    res_rnti = crnti;
    collision_manager.alloc_ded(slot_sr, cell_res_id);
  }

  return res_cfg;
}

const pucch_resource* pucch_resource_manager::reserve_csi_resource(slot_point                   slot_csi,
                                                                   rnti_t                       crnti,
                                                                   const ue_cell_configuration& ue_cell_cfg)
{
  srsran_sanity_check(slot_csi < last_sl_ind + RES_MANAGER_RING_BUFFER_SIZE,
                      "PUCCH being allocated too far into the future");

  // Get CSI specific PUCCH resource ID from the CSI meas config.
  const int cell_res_id_int = get_csi_cell_res_id(ue_cell_cfg);
  srsran_assert(cell_res_id_int >= 0, "rnti={}: CSI PUCCH resource index could not be found", crnti);
  const auto cell_res_id = static_cast<unsigned>(cell_res_id_int);

  // Get resource list of wanted slot.
  auto& ctx = slots_ctx[slot_csi.to_uint()];
  srsran_assert(cell_res_id < ctx.ues_using_pucch_res.size(),
                "rnti={}: CSI PUCCH resource index exceeds the size of the cell resource array",
                crnti);

  // Check if the wanted resource is already allocated to another UE in this slot.
  auto& res_rnti = ctx.ues_using_pucch_res[cell_res_id];
  if (res_rnti != rnti_t::INVALID_RNTI and res_rnti != crnti) {
    return nullptr;
  }

  // Search for the PUCCH resource with the correct PUCCH resource ID from the PUCCH resource list.
  const auto& pucch_res_list = ue_cell_cfg.init_bwp().ul_ded->pucch_cfg.value().pucch_res_list;
  const auto* res_cfg =
      std::find_if(pucch_res_list.begin(), pucch_res_list.end(), [cell_res_id](const pucch_resource& res) {
        return res.res_id.cell_res_id == cell_res_id;
      });
  srsran_assert(res_cfg != pucch_res_list.end(),
                "rnti={}: PUCCH resource with cell_res_id={} not found in PUCCH resource list",
                crnti,
                cell_res_id);

  // If the PUCCH res with correct ID was not allocated to the UE's RNTI, allocate it to this RNTI; otherwise, it means
  // the resource had already been allocated, just return it.
  if (res_rnti != crnti) {
    // Check for collisions.
    if (not collision_manager.can_alloc_ded(slot_csi, cell_res_id)) {
      return nullptr;
    }
    res_rnti = crnti;
    collision_manager.alloc_ded(slot_csi, cell_res_id);
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

  // We assume each UE only has 1 SR Resource Config configured.
  const unsigned cell_res_id = pucch_cfg.sr_res_list[0].pucch_res_id.cell_res_id;

  // Get resource list of wanted slot.
  auto& ctx = slots_ctx[slot_sr.to_uint()];
  srsran_assert(cell_res_id < ctx.ues_using_pucch_res.size(),
                "rnti={}: SR PUCCH resource index exceeds the size of the cell resource array",
                crnti);

  // Check if the resource is allocated to this RNTI.
  if (ctx.ues_using_pucch_res[cell_res_id] != crnti) {
    return false;
  }

  // Release the resource.
  ctx.ues_using_pucch_res[cell_res_id] = rnti_t::INVALID_RNTI;
  collision_manager.free_ded(slot_sr, cell_res_id);
  return true;
}

bool pucch_resource_manager::release_csi_resource(slot_point                   slot_csi,
                                                  rnti_t                       crnti,
                                                  const ue_cell_configuration& ue_cell_cfg)
{
  srsran_sanity_check(slot_csi < last_sl_ind + RES_MANAGER_RING_BUFFER_SIZE,
                      "PUCCH being allocated too far into the future");

  // Get CSI specific PUCCH resource ID from the CSI meas config.
  const int cell_res_id_int = get_csi_cell_res_id(ue_cell_cfg);
  srsran_assert(cell_res_id_int >= 0, "rnti={}: CSI PUCCH resource index could not be found", crnti);
  const auto cell_res_id = static_cast<unsigned>(cell_res_id_int);

  // Get resource list of wanted slot.
  auto& ctx = slots_ctx[slot_csi.to_uint()];
  srsran_assert(cell_res_id < ctx.ues_using_pucch_res.size(),
                "rnti={}: SR PUCCH resource index exceeds the size of the cell resource array",
                crnti);

  // Check if the resource is allocated to this RNTI.
  if (ctx.ues_using_pucch_res[cell_res_id] != crnti) {
    return false;
  }

  // Release the resource.
  ctx.ues_using_pucch_res[cell_res_id] = rnti_t::INVALID_RNTI;
  collision_manager.free_ded(slot_csi, cell_res_id);
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

  // Get the array of resources for the specific UE.
  const auto& ue_res_set_id_list = pucch_cfg.pucch_res_set[pucch_res_set_idx_to_uint(res_set_idx)].pucch_res_id_list;

  // [Implementation-defined] Format 0/1 resources are at the beginning of the list, while Format 2/3/4 are at the end.
  const bool is_f0_and_f2 = pucch_cfg.pucch_res_list.front().format == pucch_format::FORMAT_0 and
                            pucch_cfg.pucch_res_list.back().format == pucch_format::FORMAT_2;

  // For PUCCH F0 and F2, we don't use the last 2 resources of the PUCCH resource set; these are reserved for CSI and SR
  // slots and should only be picked for through the specific PUCCH resource indicator.
  const unsigned nof_eligible_resource = is_f0_and_f2 ? ue_res_set_id_list.size() - 2U : ue_res_set_id_list.size();
  srsran_assert(nof_eligible_resource >= 1U,
                "rnti={}: Not enough eligible resources from PUCCH resource set={}",
                crnti,
                pucch_res_set_idx_to_uint(res_set_idx));

  // Get context of wanted slot.
  auto& ctx = slots_ctx[slot_harq.to_uint()];

  std::optional<uint8_t> available_r_pucch;
  for (uint8_t r_pucch = 0; r_pucch != nof_eligible_resource; ++r_pucch) {
    const unsigned cell_res_id = ue_res_set_id_list[r_pucch].cell_res_id;

    srsran_assert(cell_res_id < ctx.ues_using_pucch_res.size(),
                  "PUCCH resource index from PUCCH resource set exceeds the size of the cell resource array");
    auto& res_rnti = ctx.ues_using_pucch_res[cell_res_id];

    if (res_rnti == crnti) {
      // If a resource is already allocated to this RNTI, use it.
      available_r_pucch = r_pucch;
      break;
    }

    if (not available_r_pucch.has_value() and res_rnti == rnti_t::INVALID_RNTI and
        collision_manager.can_alloc_ded(slot_harq, cell_res_id)) {
      // Else, keep track of the first available resource.
      available_r_pucch = r_pucch;
    }
  }

  const auto& pucch_res_list = pucch_cfg.pucch_res_list;

  // If there is an available resource, allocate it.
  if (available_r_pucch.has_value()) {
    const unsigned r_pucch = available_r_pucch.value();
    // Get the PUCCH resource ID from the PUCCH resource indicator and the PUCCH resource set.
    const unsigned cell_res_id = ue_res_set_id_list[r_pucch].cell_res_id;

    // Search for the PUCCH resource with the correct PUCCH resource ID from the PUCCH resource list.
    const auto* res_cfg =
        std::find_if(pucch_res_list.begin(), pucch_res_list.end(), [cell_res_id](const pucch_resource& res) {
          return res.res_id.cell_res_id == cell_res_id;
        });
    srsran_sanity_check(res_cfg != pucch_res_list.end(),
                        "rnti={}: PUCCH resource with cell_res_id={} not found in PUCCH resource list",
                        crnti,
                        cell_res_id);

    // Allocate it and return it.
    ctx.ues_using_pucch_res[cell_res_id] = crnti;
    collision_manager.alloc_ded(slot_harq, cell_res_id);
    return pucch_harq_resource_alloc_record{.resource = res_cfg, .pucch_res_indicator = available_r_pucch.value()};
  }
  return pucch_harq_resource_alloc_record{.resource = nullptr};
}

const pucch_resource* pucch_resource_manager::reserve_harq_resource_by_res_indicator(slot_point          slot_harq,
                                                                                     rnti_t              crnti,
                                                                                     unsigned            d_pri,
                                                                                     const pucch_config& pucch_cfg,
                                                                                     pucch_res_set_idx   res_set_idx)
{
  srsran_sanity_check(slot_harq < last_sl_ind + RES_MANAGER_RING_BUFFER_SIZE,
                      "PUCCH being allocated too far into the future");

  // Retrieve the PUCCH resource set.
  const auto& ue_res_id_set_for_harq =
      pucch_cfg.pucch_res_set[pucch_res_set_idx_to_uint(res_set_idx)].pucch_res_id_list;

  // Make sure the resource indicator points to a valid resource.
  if (d_pri >= ue_res_id_set_for_harq.size()) {
    return nullptr;
  }

  // Get PUCCH resource ID from the PUCCH resource set.
  // [Implementation-defined] We assume at most 8 resources per resource set. If this is the case, r_pucch = d_pri.
  const auto pucch_res_id = ue_res_id_set_for_harq[d_pri];

  // [Implementation-defined] Format 0/1 resources are at the beginning of the list, while Format 2/3/4 are at the end.
  const bool is_f0_and_f2 = pucch_cfg.pucch_res_list.front().format == pucch_format::FORMAT_0 and
                            pucch_cfg.pucch_res_list.back().format == pucch_format::FORMAT_2;

  // For Format 0 and Format 2, the resources indexed by PUCCH res. indicators >= ue_res_id_set_for_harq.size() - 2 are
  // reserved for CSI and SR slots. In the case, we don't need to reserve these in the PUCCH resource manager, we only
  // need to return the resources.
  if (is_f0_and_f2 and d_pri >= ue_res_id_set_for_harq.size() - 2U) {
    const auto* res_cfg = std::find_if(
        pucch_cfg.pucch_res_list.begin(), pucch_cfg.pucch_res_list.end(), [pucch_res_id](const pucch_resource& res) {
          return res.res_id.ue_res_id == pucch_res_id.ue_res_id;
        });
    return res_cfg != pucch_cfg.pucch_res_list.end() ? &(*res_cfg) : nullptr;
  }

  // Get resource list of wanted slot.
  slot_context& ctx = slots_ctx[slot_harq.to_uint()];

  // Get PUCCH resource ID from the PUCCH resource set.
  const unsigned cell_res_id = pucch_res_id.cell_res_id;
  srsran_assert(cell_res_id < ctx.ues_using_pucch_res.size(),
                "rnti={}: PUCCH resource index from PUCCH resource set exceeds the size of the cell resource array",
                crnti);

  // Check first if the wanted PUCCH resource is available.
  auto& res_rnti = ctx.ues_using_pucch_res[cell_res_id];
  if (res_rnti != rnti_t::INVALID_RNTI and res_rnti != crnti) {
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
                crnti,
                cell_res_id);

  if (res_rnti == crnti) {
    // If the resource is already allocated to this RNTI, just return it.
    return res_cfg;
  }

  // Check for collisions.
  if (not collision_manager.can_alloc_ded(slot_harq, cell_res_id)) {
    return nullptr;
  }

  // Allocate the resource to this RNTI.
  res_rnti = crnti;
  collision_manager.alloc_ded(slot_harq, cell_res_id);
  return res_cfg;
}

bool pucch_resource_manager::release_harq_resource(slot_point          slot_harq,
                                                   rnti_t              crnti,
                                                   const pucch_config& pucch_cfg,
                                                   pucch_res_set_idx   res_set_idx)
{
  srsran_sanity_check(slot_harq < last_sl_ind + RES_MANAGER_RING_BUFFER_SIZE,
                      "PUCCH being allocated too far into the future");

  // Get the array of resources for the specific UE.
  const auto& ue_res_set_id_list = pucch_cfg.pucch_res_set[pucch_res_set_idx_to_uint(res_set_idx)].pucch_res_id_list;

  // [Implementation-defined] Format 0/1 resources are at the beginning of the list, while Format 2/3/4 are at the end.
  const bool is_f0_and_f2 = pucch_cfg.pucch_res_list.front().format == pucch_format::FORMAT_0 and
                            pucch_cfg.pucch_res_list.back().format == pucch_format::FORMAT_2;

  // For PUCCH F0 and F2, we don't use the last 2 resources of the PUCCH resource set; these are reserved for CSI and SR
  // slots and should only be picked for through the specific PUCCH resource indicator.
  const unsigned nof_eligible_resource = is_f0_and_f2 ? ue_res_set_id_list.size() - 2U : ue_res_set_id_list.size();
  srsran_assert(nof_eligible_resource >= 1U,
                "rnti={}: Not enough eligible resources from PUCCH resource set={}",
                crnti,
                pucch_res_set_idx_to_uint(res_set_idx));

  // Get resource list of wanted slot.
  slot_context& ctx = slots_ctx[slot_harq.to_uint()];

  for (unsigned r_pucch = 0; r_pucch != nof_eligible_resource; ++r_pucch) {
    const unsigned cell_res_id = ue_res_set_id_list[r_pucch].cell_res_id;
    srsran_assert(cell_res_id < ctx.ues_using_pucch_res.size(),
                  "rnti={}: HARQ PUCCH resource index exceeds the size of the cell resource array");
    auto& res_rnti = ctx.ues_using_pucch_res[cell_res_id];

    if (res_rnti == crnti) {
      // Release the resource.
      res_rnti = rnti_t::INVALID_RNTI;
      collision_manager.free_ded(slot_harq, cell_res_id);
      return true;
    }
  }

  return false;
}
