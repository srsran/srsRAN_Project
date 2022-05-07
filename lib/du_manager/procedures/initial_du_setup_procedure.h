
#ifndef SRSGNB_INITIAL_DU_SETUP_PROCEDURE_H
#define SRSGNB_INITIAL_DU_SETUP_PROCEDURE_H

#include "../converters/mac_cell_configuration_helpers.h"
#include "du_manager_context.h"
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

    // Initiate F1 Setup.
    CORO_AWAIT_VALUE(du_ctxt.f1_setup_result, cfg.f1ap->f1ap_du_setup_request(cfg.setup_params));

    // Handle F1 setup result.
    if (du_ctxt.f1_setup_result.result.has_value()) {
      // In case of F1 Setup Response.
      handle_f1_setup_response(du_ctxt.f1_setup_result.result.value());
    } else {
      handle_f1_setup_failure(du_ctxt.f1_setup_result.result.error());
    }

    // Configure DU Cells.
    // TODO: remove any mac_cell_cfg initialization values, which are present only to let the tests pass.
    mac_cell_creation_request mac_cell_cfg = test_helpers::make_default_mac_cell_creation_request();
    cfg.mac_cell_mng->add_cell(mac_cell_cfg);

    // Activate DU Cells.
    cfg.mac_cell_mng->get_cell_manager(to_du_cell_index(0)).start();

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

  void handle_f1_setup_failure(const asn1::f1ap::f1_setup_fail_s& failure)
  {
    // TODO
  }

  const du_manager_config_t& cfg;
  du_manager_context&        du_ctxt;
};

} // namespace srsgnb

#endif // SRSGNB_INITIAL_DU_SETUP_PROCEDURE_H
