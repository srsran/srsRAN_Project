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

static bool has_pusch_grant_allocated(unsigned crnti, span<const ul_sched_info> puschs)
{
  const auto it = std::find_if(
      puschs.begin(), puschs.end(), [crnti](const ul_sched_info& pusch) { return pusch.pusch_cfg.rnti == crnti; });
  return it != puschs.end();
}

void pucch_scheduler_impl::run_slot(cell_resource_allocator& cell_alloc, slot_point sl_tx)
{
  // Iterate over the users to check for SR opportunities.
  for (auto& user : ues) {
    // Check if the slot is UL enabled.
    if (not cell_cfg.is_ul_enabled(sl_tx)) {
      logger.warning("SCHED: SR PUCCH occasion not scheduled as current slot is not UL enabled.");
      return;
    }

    // Check if UE has dedicated resources.
    srsgnb_assert(user.nof_cells() > 0, "pCell not configured");
    auto& ue_cell = user.get_pcell();
    srsgnb_assert(
        ue_cell.cfg().cfg_dedicated().ul_config.has_value() and
            ue_cell.cfg().cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.has_value() and
            not ue_cell.cfg().cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value().pucch_res_list.empty() and
            not ue_cell.cfg().cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value().sr_res_list.empty(),
        "SCHED: PUCCH ded resouces not configured.");

    const auto& sr_resource_cfg_list =
        ue_cell.cfg().cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value().sr_res_list;

    for (const auto& sr_res : sr_resource_cfg_list) {
      srsgnb_assert(sr_res.period >= sr_periodicity::sl_1, "Minimum supported SR perdiodicity is 1 slot.");

      // Check if this slot corresponds to an SR opportunity for the UE.
      if ((sl_tx - sr_res.offset).to_uint() % sr_periodicity_to_slot(sr_res.period) == 0) {
        const unsigned SR_SLOT_DELAY = 0;
        auto&          slot_alloc    = cell_alloc[SR_SLOT_DELAY];
        // If there is a PUSCH allocated for this UE, do not allocate any PUCCH SR grants.
        if (has_pusch_grant_allocated(user.crnti, slot_alloc.result.ul.puschs)) {
          logger.debug("SCHED: SR allocation skipped for RNTI {:#x} due to PUSCH grant allocated.", user.crnti);
          return;
        }

        pucch_alloc.pucch_allocate_sr_opportunity(slot_alloc, user.crnti, ue_cell.cfg());
      }
    }
  }
}
