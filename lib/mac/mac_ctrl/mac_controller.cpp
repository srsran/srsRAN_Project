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

#include "mac_controller.h"
#include "../rnti_manager.h"
#include "mac_ue_removal_procedure.h"
#include "ue_creation_procedure.h"
#include "ue_reconfiguration_procedure.h"
#include "srsran/mac/mac_clock_controller.h"

using namespace srsran;

mac_controller::mac_controller(const mac_control_config&   cfg_,
                               mac_ul_configurator&        ul_unit_,
                               mac_dl_configurator&        dl_unit_,
                               rnti_manager&               rnti_table_,
                               mac_scheduler_configurator& sched_cfg_) :
  cfg(cfg_),
  logger(cfg.logger),
  ul_unit(ul_unit_),
  dl_unit(dl_unit_),
  rnti_table(rnti_table_),
  sched_cfg(sched_cfg_),
  time_ctrl(cfg.time_source),
  metrics(cfg.metrics, cfg.ctrl_exec, cfg.time_source.get_timer_manager(), logger)
{
}

mac_cell_controller& mac_controller::add_cell(const mac_cell_creation_request& cell_add_req)
{
  // Add new cell to track timing.
  auto cell_time_source = time_ctrl.add_cell(cell_add_req.cell_index);

  // Add cell to metrics reports.
  auto cell_metrics_cfg = metrics.add_cell(cell_add_req.cell_index, cell_add_req.scs_common, *cell_time_source);

  // > Fill sched cell configuration message and pass it to the scheduler.
  sched_cfg.add_cell(mac_scheduler_cell_creation_request{
      cell_add_req, cell_metrics_cfg.report_period, cell_metrics_cfg.sched_notifier});

  // > Create MAC Cell DL Handler.
  return dl_unit.add_cell(cell_add_req,
                          mac_cell_config_dependencies{std::move(cell_time_source),
                                                       cell_metrics_cfg.report_period,
                                                       cell_metrics_cfg.mac_notifier});
}

void mac_controller::remove_cell(du_cell_index_t cell_index)
{
  // > Remove cell from MAC Cell Handler.
  dl_unit.remove_cell(cell_index);

  // > Remove cell from scheduler.
  sched_cfg.remove_cell(cell_index);

  // > Remove cell from metrics reports.
  metrics.rem_cell(cell_index);
}

mac_cell_time_mapper& mac_controller::get_time_mapper(du_cell_index_t cell_index)
{
  return dl_unit.get_time_mapper(cell_index);
}

async_task<mac_ue_create_response> mac_controller::handle_ue_create_request(const mac_ue_create_request& msg)
{
  return launch_async<mac_ue_create_request_procedure>(msg, cfg, *this, ul_unit, dl_unit, sched_cfg);
}

async_task<mac_ue_delete_response> mac_controller::handle_ue_delete_request(const mac_ue_delete_request& msg)
{
  return launch_async<mac_ue_removal_procedure>(msg, cfg, *this, ul_unit, dl_unit, sched_cfg);
}

async_task<mac_ue_reconfiguration_response>
mac_controller::handle_ue_reconfiguration_request(const mac_ue_reconfiguration_request& msg)
{
  return launch_async<mac_ue_reconfiguration_procedure>(msg, cfg, ul_unit, dl_unit, sched_cfg);
}

void mac_controller::handle_ue_config_applied(du_ue_index_t ue_index)
{
  srsran_assert(ue_db.contains(ue_index), "Invalid ue_index={}", fmt::underlying(ue_index));

  ul_unit.handle_ue_config_applied(ue_index);

  sched_cfg.handle_ue_config_applied(ue_index);
}

rnti_t mac_controller::add_ue(du_ue_index_t ue_index, du_cell_index_t cell_index, rnti_t tc_rnti)
{
  srsran_assert(is_du_ue_index_valid(ue_index), "Invalid ue_index={}", fmt::underlying(ue_index));

  if (ue_db.contains(ue_index)) {
    // UE already existed with same ue_index.
    return rnti_t::INVALID_RNTI;
  }

  // Create UE object.
  ue_db.emplace(ue_index);
  mac_ue_context& u = ue_db[ue_index];
  u.du_ue_index     = ue_index;
  u.pcell_idx       = cell_index;

  if (tc_rnti != rnti_t::INVALID_RNTI) {
    // We reuse the TC-RNTI as the C-RNTI, if it was already chosen.
    u.rnti = tc_rnti;
  } else {
    // Otherwise, we generate a new C-RNTI.
    u.rnti = rnti_table.allocate();
  }

  // Update RNTI -> UE index map.
  if (not rnti_table.add_ue(u.rnti, ue_index)) {
    logger.error(
        "ue={} rnti={}: The update of the RNTI table overwrote a previous entry", fmt::underlying(ue_index), u.rnti);
    rnti_table.rem_ue(u.rnti);
    return rnti_t::INVALID_RNTI;
  }
  return u.rnti;
}

void mac_controller::remove_ue(du_ue_index_t ue_index)
{
  if (not ue_db.contains(ue_index)) {
    logger.warning("Failed to find ue_index={:#x}", fmt::underlying(ue_index));
    return;
  }

  // Remove RNTI from the RNTI entry.
  rnti_table.rem_ue(ue_db[ue_index].rnti);

  ue_db.erase(ue_index);
}

mac_ue_context* mac_controller::find_ue(du_ue_index_t ue_index)
{
  return ue_db.contains(ue_index) ? &ue_db[ue_index] : nullptr;
}

mac_ue_context* mac_controller::find_by_rnti(rnti_t rnti)
{
  return find_ue(rnti_table[rnti]);
}
