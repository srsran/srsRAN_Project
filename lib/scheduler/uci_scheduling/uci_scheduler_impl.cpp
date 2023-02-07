/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "uci_scheduler_impl.h"
#include "../cell/resource_grid.h"
#include "uci_allocator_impl.h"

using namespace srsgnb;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uci_scheduler_impl::uci_scheduler_impl(const cell_configuration& cell_cfg_, uci_allocator& uci_alloc_, ue_list& ues_) :
  cell_cfg(cell_cfg_), uci_alloc(uci_alloc_), ues(ues_), logger(srslog::fetch_basic_logger("SCHED"))
{
}

uci_scheduler_impl::~uci_scheduler_impl() = default;

void uci_scheduler_impl::run_slot(cell_resource_allocator& cell_alloc, slot_point sl_tx)
{
  uci_alloc.slot_indication(sl_tx);

  // Iterate over the users to check for SR opportunities.
  for (auto& user : ues) {
    // Check if the slot is UL enabled.
    if (not cell_cfg.is_ul_enabled(sl_tx)) {
      return;
    }

    // At this point, we assume the config validator ensures there is pCell.
    auto& ue_cell = user.get_pcell();

    // > Schedule SR grants.
    // At this point, we assume the UE has a \c ul_config, a \c pucch_cfg and a \c sr_res_list.
    const auto& sr_resource_cfg_list =
        ue_cell.cfg().cfg_dedicated().ul_config.value().init_ul_bwp.pucch_cfg.value().sr_res_list;

    for (const auto& sr_res : sr_resource_cfg_list) {
      srsgnb_assert(sr_res.period >= sr_periodicity::sl_1, "Minimum supported SR perdiodicity is 1 slot.");

      // Check if this slot corresponds to an SR opportunity for the UE.
      if ((sl_tx - sr_res.offset).to_uint() % sr_periodicity_to_slot(sr_res.period) == 0) {
        const unsigned SR_SLOT_DELAY = 0;
        auto&          slot_alloc    = cell_alloc[SR_SLOT_DELAY];

        // It is up to the UCI allocator to verify whether SR allocation can be skipped due to an existing PUCCH grant.
        uci_alloc.uci_allocate_sr_opportunity(slot_alloc, user.crnti, ue_cell.cfg());
      }
    }

    // TODO: This check can be removed once the CSI configuration will be enabled.
    if (ue_cell.cfg().cfg_dedicated().csi_meas_cfg.has_value()) {
      // We assume we only use the first CSI report configuration.
      const unsigned csi_report_cfg_idx = 0;
      const auto&    csi_report_cfg =
          ue_cell.cfg().cfg_dedicated().csi_meas_cfg.value().csi_report_cfg_list[csi_report_cfg_idx];

      // > Scheduler CSI grants.
      unsigned csi_offset =
          variant_get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(csi_report_cfg.report_cfg_type)
              .report_slot_offset;
      unsigned csi_period = csi_report_periodicity_to_uint(
          variant_get<csi_report_config::periodic_or_semi_persistent_report_on_pucch>(csi_report_cfg.report_cfg_type)
              .report_slot_period);

      // Check if this slot corresponds to a CSI opportunity for the UE.
      if ((sl_tx - csi_offset).to_uint() % csi_period == 0) {
        // We do not allocate the CSI head of time.
        const unsigned CSI_SLOT_DELAY = 0;
        auto&          slot_alloc     = cell_alloc[CSI_SLOT_DELAY];

        uci_alloc.uci_allocate_csi_opportunity(slot_alloc, user.crnti, ue_cell.cfg());
      }
    }
  }
}
