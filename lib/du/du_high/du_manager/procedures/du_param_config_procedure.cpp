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

#include "du_param_config_procedure.h"
#include "../converters/f1ap_configuration_helpers.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/async/async_no_op_task.h"

using namespace srsran;
using namespace srs_du;

du_param_config_procedure::du_param_config_procedure(const du_param_config_request& request_,
                                                     const du_manager_params&       du_params_,
                                                     du_cell_manager&               du_cells_) :
  request(request_), du_params(du_params_), du_cells(du_cells_), logger(srslog::fetch_basic_logger("DU-MNG"))
{
}

void du_param_config_procedure::operator()(coro_context<async_task<du_param_config_response>>& ctx)
{
  CORO_BEGIN(ctx);

  // Update DU cell configs.
  if (not handle_cell_config_updates()) {
    resp.success = false;
    CORO_EARLY_RETURN(resp);
  }

  for (; next_cell_idx != changed_cells.size(); ++next_cell_idx) {
    // Reconfigure cell in the MAC.
    CORO_AWAIT_VALUE(mac_cell_reconfig_response macresp, handle_mac_cell_update(next_cell_idx));
    if (not macresp.sib1_updated) {
      resp.success = false;
      CORO_EARLY_RETURN(resp);
    }
  }

  // Initiate F1AP gNB-DU Configuration Update
  CORO_AWAIT_VALUE(gnbdu_config_update_response f1resp, handle_f1_gnbdu_config_update());

  resp.success = f1resp.result;
  CORO_RETURN(resp);
}

bool du_param_config_procedure::handle_cell_config_updates()
{
  for (const du_cell_param_config_request& cell_to_update : request.cells) {
    du_cell_index_t cell_index = du_cells.get_cell_index(cell_to_update.nr_cgi);
    if (cell_index == INVALID_DU_CELL_INDEX) {
      logger.warning("Discarding cell {} changes. Cause: No cell with the provided CGI was found",
                     cell_to_update.nr_cgi.nci);
      return false;
    }

    du_cell_config& cell_cfg = du_cells.get_cell_cfg(cell_index);
    if (cell_to_update.ssb_pwr_mod.has_value() and
        cell_to_update.ssb_pwr_mod.value() != cell_cfg.ssb_cfg.ssb_block_power) {
      // SSB power changed.
      cell_cfg.ssb_cfg.ssb_block_power = cell_to_update.ssb_pwr_mod.value();

      // Mark that cell was changed.
      changed_cells.push_back(cell_index);
    }
  }

  return true;
}

async_task<gnbdu_config_update_response> du_param_config_procedure::handle_f1_gnbdu_config_update()
{
  gnbdu_config_update_request f1_req{};

  f1_req.cells_to_mod.resize(changed_cells.size());
  for (unsigned i = 0, e = changed_cells.size(); i != e; ++i) {
    const du_cell_config& cell_cfg = du_cells.get_cell_cfg(changed_cells[i]);

    f1_req.cells_to_mod[i].cell_info   = make_f1ap_du_cell_info(cell_cfg);
    f1_req.cells_to_mod[i].du_sys_info = make_f1ap_du_sys_info(cell_cfg);
  }

  return du_params.f1ap.conn_mng.handle_du_config_update(f1_req);
}

async_task<mac_cell_reconfig_response> du_param_config_procedure::handle_mac_cell_update(unsigned changed_cell_idx)
{
  du_cell_index_t       cell_index = changed_cells[changed_cell_idx];
  const du_cell_config& cell_cfg   = du_cells.get_cell_cfg(cell_index);

  mac_cell_reconfig_request req;

  // Pack new system information.
  std::vector<byte_buffer> bcch_msgs = make_asn1_rrc_cell_bcch_dl_sch_msgs(cell_cfg);

  // Note: For now only SIB1 changes are supported.
  req.new_sib1_buffer = std::move(bcch_msgs[0]);

  return du_params.mac.cell_mng.get_cell_controller(cell_index).reconfigure(req);
}
