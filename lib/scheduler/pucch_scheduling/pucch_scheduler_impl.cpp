/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pucch_scheduler_impl.h"
#include "../support/pucch/pucch_default_resource.h"

using namespace srsgnb;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

pucch_scheduler_impl::pucch_scheduler_impl(const cell_configuration& cell_cfg_,
                                           pucch_allocator&          pucch_alloc_,
                                           ue_list&                  ues_) :
  cell_cfg(cell_cfg_), pucch_alloc(pucch_alloc_), ues(ues_), logger(srslog::fetch_basic_logger("MAC"))
{
}

pucch_scheduler_impl::~pucch_scheduler_impl() = default;

static bool has_pusch_grant_allocated(unsigned crnti, const span<ul_sched_info> puschs)
{
  auto it = std::find_if(
      puschs.begin(), puschs.end(), [crnti](ul_sched_info& pusch) { return pusch.pusch_cfg.rnti == crnti; });
  return it == puschs.end() ? false : true;
}

void pucch_scheduler_impl::sched_ue_sr_opportunity(cell_resource_allocator& cell_alloc, ue& user, slot_point sl_tx)
{
  // Check if the slot is UL enabled.
  if (not cell_cfg.is_ul_enabled(sl_tx)) {
    logger.warning("SCHED: SR PUCCH occasion not scheduled as current slot is not UL enabled.");
    return;
  }

  // Check if UE has dedicated resources.
  if (user.nof_cells() == 0) {
    logger.warning("SCHED: pCell not configured for RNTI {:#x}- SR allocation will be skipped.", user.crnti);
    return;
  }
  auto& ue_cell = user.get_pcell();
  if (not ue_cell.cfg().cfg_dedicated().ul_config.has_value() or
      not ue_cell.cfg().cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.has_value() or
      ue_cell.cfg().cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value().pucch_res_list.empty() or
      ue_cell.cfg().cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value().sr_res_list.empty()) {
    logger.warning("SCHED: PUCCH ded resouces not configured for RNTI {:#x}- SR allocation will be skipped.",
                   user.crnti);
    return;
  }

  const auto& sr_resource_cfg_list =
      ue_cell.cfg().cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value().sr_res_list;

  for (const auto& sr_res : sr_resource_cfg_list) {
    srsgnb_assert(sr_res.period >= sr_periodicity::sl_1, "Minimum supported SR perdiodicity is 1 slot.");

    // Check if this slot corresponds to an SR opportunity for the UE.
    if ((static_cast<unsigned>(sl_tx.to_uint()) - sr_res.offset) % sr_periodicity_to_slot(sr_res.period) == 0) {
      auto& slot_alloc = cell_alloc[0];
      // If there is a PUSCH allocated for this UE, do not allocate any PUCCH SR grants.
      if (has_pusch_grant_allocated(user.crnti, slot_alloc.result.ul.puschs)) {
        logger.debug("SCHED: SR allocation skipped for RNTI {:#x} due to PUSCH grant allocated.", user.crnti);
        return;
      }

      pucch_alloc.pucch_allocate_sr_opportunity(slot_alloc, user.crnti, ue_cell.cfg());
    }
  }
}

void pucch_scheduler_impl::run_slot(cell_resource_allocator& cell_alloc, slot_point sl_tx)
{
  // Return if there are no users to serve.
  if (ues.empty()) {
    return;
  }

  // Iterate over the users to check for SR opportunities.
  for (auto& ue : ues) {
    sched_ue_sr_opportunity(cell_alloc, ue, sl_tx);
  }
}
