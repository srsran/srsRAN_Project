/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_INITIAL_DU_SETUP_PROCEDURE_H
#define SRSGNB_INITIAL_DU_SETUP_PROCEDURE_H

#include "../converters/f1_procedure_helpers.h"
#include "../converters/mac_cell_configuration_helpers.h"
#include "../du_manager_context.h"
#include "srsgnb/asn1/f1ap.h"
#include "srsgnb/f1_interface/f1ap_du.h"
#include "srsgnb/support/async/async_task.h"

namespace srsgnb {

class initial_du_setup_procedure
{
public:
  initial_du_setup_procedure(const du_manager_config_t& cfg_, du_manager_context& du_ctxt_) :
    cfg(cfg_), du_ctxt(du_ctxt_)
  {}

  void operator()(coro_context<async_task<void> >& ctx)
  {
    CORO_BEGIN(ctx);

    du_ctxt = {};

    f1ap::fill_f1_setup_request_message(request_msg, cfg.setup_params);

    // Initiate F1 Setup.
    CORO_AWAIT_VALUE(response_msg, cfg.f1ap->handle_f1ap_setup_request(request_msg));

    // Handle F1 setup result.
    if (response_msg.success) {
      // In case of F1 Setup Response.
      handle_f1_setup_response(response_msg.msg);
    }

    // Configure DU Cells.
    // TODO: remove any mac_cell_cfg initialization values, which are present only to let the tests pass.
    mac_cell_creation_request mac_cell_cfg = test_helpers::make_default_mac_cell_creation_request();
    cfg.mac_cell_mng->add_cell(mac_cell_cfg);

    // Activate DU Cells.
    cfg.mac_cell_mng->get_cell_controller(to_du_cell_index(0)).start();

    CORO_RETURN();
  }

private:
  void handle_f1_setup_response(const asn1::f1ap::f1_setup_resp_s& resp)
  {
    // TODO

    if (resp->cells_to_be_activ_list_present) {
      for (const auto& cell : resp->cells_to_be_activ_list.value) {
        const asn1::f1ap::cells_to_be_activ_list_item_s& item = cell->cells_to_be_activ_list_item();
        (void)item;
        // TODO: Handle cells to activate.
      }
    }
  }

  const du_manager_config_t& cfg;
  du_manager_context&        du_ctxt;

  f1_setup_request_message  request_msg  = {};
  f1_setup_response_message response_msg = {};
};

} // namespace srsgnb

#endif // SRSGNB_INITIAL_DU_SETUP_PROCEDURE_H
