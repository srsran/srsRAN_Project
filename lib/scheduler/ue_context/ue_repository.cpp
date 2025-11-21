/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ue_repository.h"
#include "srsran/scheduler/resource_grid_util.h"
#include "srsran/srslog/srslog.h"

using namespace srsran;

// class ue_cell_repository

ue_cell_repository::ue_cell_repository(du_cell_index_t cell_idx_, srslog::basic_logger& logger_) :
  cell_idx(cell_idx_), logger(logger_)
{
  rnti_to_ue_index_lookup.reserve(MAX_NOF_DU_UES);
  ues.reserve(MAX_NOF_DU_UES);
}

void ue_cell_repository::add_ue(ue_cell& u)
{
  rnti_t        rnti     = u.rnti();
  du_ue_index_t ue_index = u.ue_index;
  bool          ret      = ues.insert(u.ue_index, &u);
  srsran_assert(ret, "UE with duplicate index being added to the cell UE repository");
  auto res = rnti_to_ue_index_lookup.insert(std::make_pair(rnti, ue_index));
  srsran_assert(res.second, "UE with duplicate RNTI being added to the cell UE repository");
}

void ue_cell_repository::rem_ue(du_ue_index_t ue_index)
{
  if (not ues.contains(ue_index)) {
    logger.error("ue={} : UE not found in the cell UE repository", fmt::underlying(ue_index));
  }
  const ue_cell&      u      = *ues[ue_index];
  const rnti_t        crnti  = u.rnti();
  const du_ue_index_t ue_idx = u.ue_index;

  // Remove UE from lookup.
  auto it = rnti_to_ue_index_lookup.find(crnti);
  if (it != rnti_to_ue_index_lookup.end()) {
    rnti_to_ue_index_lookup.erase(it);
  } else {
    logger.error("ue={} rnti={}: UE with provided c-rnti not found in RNTI-to-UE-index lookup table.",
                 fmt::underlying(ue_idx),
                 crnti);
  }
  // Take the ue cell from the repository.
  ues.erase(ue_idx);
}

// class ue_repository

ue_repository::ue_repository() : logger(srslog::fetch_basic_logger("SCHED")), ues_to_destroy(MAX_NOF_DU_UES)
{
  rnti_to_ue_index_lookup.reserve(MAX_NOF_DU_UES);
}

ue_repository::~ue_repository()
{
  for (auto& u : ues_to_rem) {
    u.second.release();
  }
}

/// \brief This function checks whether it is safe to remove a UE. Currently we verify that the UE has no DL or UL
/// HARQ awaiting an ACK.
static bool is_ue_ready_for_removal(ue& u)
{
  // Ensure that there no currently active HARQs.
  unsigned nof_ue_cells = u.nof_cells();
  for (unsigned cell_idx = 0; cell_idx != nof_ue_cells; ++cell_idx) {
    const ue_cell& c = u.get_cell((ue_cell_index_t)cell_idx);
    for (unsigned i = 0, e = c.harqs.nof_dl_harqs(); i != e; ++i) {
      if (c.harqs.dl_harq(to_harq_id(i)).has_value()) {
        return false;
      }
    }
    for (unsigned i = 0, e = c.harqs.nof_ul_harqs(); i != e; ++i) {
      if (c.harqs.ul_harq(to_harq_id(i)).has_value()) {
        return false;
      }
    }
  }

  return true;
}

void ue_repository::slot_indication(slot_point sl_tx)
{
  last_sl_tx = sl_tx;

  for (std::pair<slot_point, ue_config_delete_event>& p : ues_to_rem) {
    auto& rem_ev = p.second;
    if (not rem_ev.valid()) {
      // Already removed.
      continue;
    }
    if (p.first > sl_tx) {
      // UE is not yet ready to be removed as there may be still pending allocations for it in the resource grid.
      continue;
    }

    const du_ue_index_t ue_idx = rem_ev.ue_index();
    if (not ues.contains(ue_idx)) {
      logger.error("ue={}: Unexpected UE removal from UE repository", fmt::underlying(ue_idx));
      rem_ev.reset();
      continue;
    }
    ue&    u     = *ues[ue_idx];
    rnti_t crnti = u.crnti;

    // Check if UEs can be safely removed.
    if (not is_ue_ready_for_removal(u)) {
      continue;
    }

    // Remove UE from the repository.
    rem_ue(u);

    // Marks UE config removal as complete.
    rem_ev.reset();

    logger.debug("ue={} rnti={}: UE has been successfully removed.", fmt::underlying(ue_idx), crnti);
  }

  // In case the elements at the front of the ring has been marked for removal, pop them from the queue.
  while (not ues_to_rem.empty() and not ues_to_rem[0].second.valid()) {
    ues_to_rem.pop();
  }

  // Update state of existing UEs.
  dl_lc_ch_sys.slot_indication();
  for (auto& u : ues) {
    u->slot_indication(sl_tx);
  }
}

ue_cell_repository& ue_repository::add_cell(du_cell_index_t cell_index)
{
  srsran_sanity_check(not cell_ues.contains(cell_index), "Cell index {} is duplicate", fmt::underlying(cell_index));
  cell_ues.emplace(cell_index, cell_index, logger);
  return cell_ues[cell_index];
}

void ue_repository::rem_cell(du_cell_index_t cell_index)
{
  cell_ues.erase(cell_index);
}

void ue_repository::add_ue(std::unique_ptr<ue> u, logical_channel_config_list_ptr lc_cfgs)
{
  // Add UE in repository.
  const du_ue_index_t      ue_index = u->ue_index;
  const rnti_t             rnti     = u->crnti;
  const subcarrier_spacing scs      = u->get_pcell().active_bwp().dl_common->value().generic_params.scs;
  u->setup(dl_lc_ch_sys.create_ue(ue_index, scs, u->get_pcell().is_in_fallback_mode(), lc_cfgs));
  bool ret = ues.insert(ue_index, std::move(u));
  srsran_assert(ret, "UE with duplicate index being added to the repository");

  // Update RNTI -> UE index lookup.
  auto res = rnti_to_ue_index_lookup.insert(std::make_pair(rnti, ue_index));
  srsran_assert(res.second, "UE with duplicate RNTI being added to the repository");

  // Add UE in cell-specific repositories.
  auto& ue_added = ues[ue_index];
  for (unsigned i = 0, sz = ue_added->nof_cells(); i != sz; ++i) {
    auto& ue_cc = ue_added->get_cell(to_ue_cell_index(i));
    cell_ues[ue_cc.cell_index].add_ue(ue_cc);
  }
}

void ue_repository::reconfigure_ue(const ue_reconf_command& cmd, bool reestablished_)
{
  srsran_sanity_check(
      ues.contains(cmd.cfg.ue_index), "ue={} : UE not found in the repository", fmt::underlying(cmd.cfg.ue_index));

  for (ue_cell_repository& cell_repo : cell_ues) {
    if (cell_repo.contains(cmd.cfg.ue_index) and not cmd.cfg.contains(cell_repo.cell_index())) {
      // Cell has been removed from the UE configuration.
      cell_repo.rem_ue(cmd.cfg.ue_index);
    }
  }

  auto& u = ues[cmd.cfg.ue_index];
  u->handle_reconfiguration_request(cmd, reestablished_);

  for (unsigned i = 0, sz = cmd.cfg.nof_cells(); i != sz; ++i) {
    const auto&           cell_cfg   = cmd.cfg.ue_cell_cfg(to_ue_cell_index(i));
    const du_cell_index_t cell_index = cell_cfg.cell_cfg_common.cell_index;

    if (not cell_ues[cell_index].contains(u->ue_index)) {
      // New cell being instantiated.
      cell_ues[cell_index].add_ue(*u->find_cell(cell_index));
    }
  }
}

void ue_repository::schedule_ue_rem(ue_config_delete_event ev)
{
  if (contains(ev.ue_index())) {
    // Start deactivation of UE bearers.
    auto& u = ues[ev.ue_index()];
    u->deactivate();

    // Register UE for later removal.
    // We define a time window when the UE removal is not allowed, as there are pending CSI/SR PDUs in the resource
    // grid ready to be sent to the PHY. Removing the UE earlier would mean that its PUCCH resources would become
    // available to a newly created UE and there could be a PUCCH collision.
    slot_point rem_slot =
        last_sl_tx + get_max_slot_ul_alloc_delay(u->get_pcell().cfg().cell_cfg_common.ntn_cs_koffset) + 1;
    ues_to_rem.push(std::make_pair(rem_slot, std::move(ev)));
  }
}

bounded_bitset<MAX_NOF_DU_UES> ue_repository::get_ues_with_pending_newtx_data(ran_slice_id_t slice_id, bool is_dl) const
{
  if (is_dl) {
    return dl_lc_ch_sys.get_ues_with_pending_data(slice_id);
  }

  // TODO: Support for UL.
  if (ues.empty()) {
    return bounded_bitset<MAX_NOF_DU_UES>{};
  }
  auto last = ues.end();
  --last;
  bounded_bitset<MAX_NOF_DU_UES> ues_with_data(last->get()->ue_index + 1);
  ues_with_data.fill(true);
  return ues_with_data;
}

ue* ue_repository::find_by_rnti(rnti_t rnti)
{
  auto it = rnti_to_ue_index_lookup.find(rnti);
  return it != rnti_to_ue_index_lookup.end() ? ues[it->second].get() : nullptr;
}

const ue* ue_repository::find_by_rnti(rnti_t rnti) const
{
  auto it = rnti_to_ue_index_lookup.find(rnti);
  return it != rnti_to_ue_index_lookup.end() ? ues[it->second].get() : nullptr;
}

void ue_repository::destroy_pending_ues()
{
  std::unique_ptr<ue> removed_ue;
  while (ues_to_destroy.try_pop(removed_ue)) {
  }
}

void ue_repository::rem_ue(const ue& u)
{
  const rnti_t        crnti  = u.crnti;
  const du_ue_index_t ue_idx = u.ue_index;

  // Remove UE cc from the cell-specific repositories.
  for (unsigned i = 0, sz = u.nof_cells(); i != sz; ++i) {
    const auto& ue_cc = u.get_cell(to_ue_cell_index(i));
    cell_ues[ue_cc.cell_index].rem_ue(ue_idx);
  }

  // Remove UE from lookup.
  auto it = rnti_to_ue_index_lookup.find(crnti);
  if (it != rnti_to_ue_index_lookup.end()) {
    rnti_to_ue_index_lookup.erase(it);
  } else {
    logger.error("ue={} rnti={}: UE with provided c-rnti not found in RNTI-to-UE-index lookup table.",
                 fmt::underlying(ue_idx),
                 crnti);
  }

  // Take the UE from the repository and release its resources.
  auto ue_ptr = std::move(ues[ue_idx]);
  ues.erase(ue_idx);
  ue_ptr->release_resources();

  // schedule UE object destruction outside the real-time thread.
  if (not ues_to_destroy.try_push(std::move(ue_ptr))) {
    logger.warning("Failed to offload UE destruction. Performance may be affected");
  }
}

void ue_repository::handle_cell_deactivation(du_cell_index_t cell_index)
{
  for (std::unique_ptr<ue>& u : ues) {
    ue_cell* ue_cc = u->find_cell(cell_index);
    if (ue_cc == nullptr) {
      // UE does not have this cell, so we can skip it.
      continue;
    }

    // Note: We now remove the UE from the repository, indepedently of whether it is a PCell or SCell. It would be
    // very hard to handle a UE that has a config for a cell that is not active.
    rem_ue(*u);
  }

  // We may have removed UEs that were scheduled for removal in an earlier slot. We need to clean up the ues_to_rem.
  for (std::pair<slot_point, ue_config_delete_event>& p : ues_to_rem) {
    auto& rem_ev = p.second;
    if (rem_ev.valid()) {
      const du_ue_index_t ue_idx = rem_ev.ue_index();
      if (not ues.contains(ue_idx)) {
        // UE removed in the previous loop, so we need to clear this event.
        rem_ev.reset();
      }
    }
  }
}
