/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_operator_config_procedure.h"
#include "srsran/support/async/async_no_op_task.h"

using namespace srsran;
using namespace srs_du;

du_operator_config_procedure::du_operator_config_procedure(const du_operator_config_request& request_,
                                                           const du_manager_params&          du_params_,
                                                           du_cell_manager&                  du_cells_) :
  request(request_), du_params(du_params_), du_cells(du_cells_)
{
}

void du_operator_config_procedure::operator()(coro_context<async_task<du_operator_config_response>>& ctx)
{
  CORO_BEGIN(ctx);

  // Initiate F1AP gNB-DU Configuration Update
  CORO_AWAIT(handle_f1_gnbdu_config_update());

  for (; next_cell_idx != request.cells.size(); ++next_cell_idx) {
    CORO_AWAIT(handle_mac_cell_update(next_cell_idx));
  }

  CORO_RETURN(resp);
}

async_task<gnbdu_config_update_response> du_operator_config_procedure::handle_f1_gnbdu_config_update()
{
  gnbdu_config_update_request f1_req{};

  // TODO changes

  return du_params.f1ap.conn_mng.handle_gnbdu_config_update_request(f1_req);
}

async_task<bool> du_operator_config_procedure::handle_mac_cell_update(unsigned cell_list_idx)
{
  const du_operator_cell_config_request& cells_to_update = request.cells[cell_list_idx];

  du_cell_index_t cell_index = MAX_NOF_DU_CELLS;
  for (unsigned i = 0, e = du_cells.nof_cells(); i != e; ++i) {
    du_cell_config& cell_it = du_cells.get_cell_cfg(to_du_cell_index(i));
    if (cell_it.nr_cgi == cells_to_update.nr_cgi) {
      cell_index = to_du_cell_index(i);
      break;
    }
  }
  if (cell_index == MAX_NOF_DU_CELLS) {
    // cell not found.
    return launch_no_op_task(false);
  }

  mac_cell_reconfig_request req;

  // TODO changes

  // std::vector<byte_buffer> bcch_msgs = srs_du::make_asn1_rrc_cell_bcch_dl_sch_msgs(*cell_cfg);

  return du_params.mac.cell_mng.get_cell_controller(cell_index).reconfigure(req);
}
