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

#include "mac_controller.h"
#include "../rnti_manager.h"
#include "ue_creation_procedure.h"
#include "ue_delete_procedure.h"
#include "ue_reconfiguration_procedure.h"

using namespace srsran;

mac_controller::mac_controller(const mac_control_config&   cfg_,
                               mac_ul_configurator&        ul_unit_,
                               mac_dl_configurator&        dl_unit_,
                               rnti_manager&               rnti_table_,
                               mac_scheduler_configurator& sched_cfg_) :
  cfg(cfg_), logger(cfg.logger), ul_unit(ul_unit_), dl_unit(dl_unit_), rnti_table(rnti_table_), sched_cfg(sched_cfg_)
{
}

void mac_controller::add_cell(const mac_cell_creation_request& cell_cfg)
{
  // > Fill sched cell configuration message and pass it to the scheduler.
  sched_cfg.add_cell(cell_cfg);

  // > Create MAC Cell DL Handler.
  dl_unit.add_cell(cell_cfg);
}

void mac_controller::remove_cell(du_cell_index_t cell_index)
{
  // > Remove cell from MAC Cell Handler.
  dl_unit.remove_cell(cell_index);

  // > Remove cell from scheduler.
  sched_cfg.remove_cell(cell_index);
}

mac_cell_controller& mac_controller::get_cell_controller(du_cell_index_t cell_index)
{
  return dl_unit.get_cell_controller(cell_index);
}

async_task<mac_ue_create_response> mac_controller::handle_ue_create_request(const mac_ue_create_request& msg)
{
  return launch_async<mac_ue_create_request_procedure>(msg, cfg, *this, ul_unit, dl_unit, sched_cfg);
}

async_task<mac_ue_delete_response> mac_controller::handle_ue_delete_request(const mac_ue_delete_request& msg)
{
  return launch_async<mac_ue_delete_procedure>(msg, cfg, *this, ul_unit, dl_unit, sched_cfg);
}

async_task<mac_ue_reconfiguration_response>
mac_controller::handle_ue_reconfiguration_request(const mac_ue_reconfiguration_request& msg)
{
  return launch_async<mac_ue_reconfiguration_procedure>(msg, cfg, ul_unit, dl_unit, sched_cfg);
}

rnti_t mac_controller::add_ue(du_ue_index_t ue_index, du_cell_index_t cell_index, rnti_t tc_rnti)
{
  srsran_assert(is_du_ue_index_valid(ue_index), "Invalid ue_index={}", ue_index);

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
    logger.error("ue={} rnti={}: The update of the RNTI table overwrote a previous entry", ue_index, u.rnti);
    rnti_table.rem_ue(u.rnti);
    return rnti_t::INVALID_RNTI;
  }
  return u.rnti;
}

void mac_controller::remove_ue(du_ue_index_t ue_index)
{
  if (not ue_db.contains(ue_index)) {
    logger.warning("Failed to find ue_index={:#x}", ue_index);
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
