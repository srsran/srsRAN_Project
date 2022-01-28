
#ifndef SRSGNB_UE_CREATION_PROCEDURE_H
#define SRSGNB_UE_CREATION_PROCEDURE_H

#include "du_manager_context.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/rlc/rlc.h"

namespace srsgnb {

class ue_creation_procedure
{
public:
  ue_creation_procedure(du_manager_context& du_ctxt_, const du_ue_create_message& ue_create_msg) :
    du_ctxt(du_ctxt_), ue_create_cmd(ue_create_msg)
  {
    // 1. Create UE context in DU manager
    if (du_ctxt.ue_db.size() <= ue_create_msg.ue_index) {
      du_ctxt.ue_db.resize(ue_create_msg.ue_index + 1);
    }
    du_ue_context& ue_ctxt = du_ctxt.ue_db[ue_create_msg.ue_index];
    ue_ctxt.pcell_index    = ue_create_msg.cell_index;
    ue_ctxt.ue_index       = ue_create_msg.ue_index;
    ue_ctxt.crnti          = ue_create_msg.crnti;

    // 2. Initiate RLC UE creation
    rlc_ue_create_message rlc_msg{};
    rlc_msg.cell_index = ue_create_msg.cell_index;
    rlc_msg.ue_index   = ue_create_msg.ue_index;
    du_ctxt.rlc->ue_create(rlc_msg);
  }

  void rlc_ue_create_response(const rlc_ue_create_response_message& resp)
  {
    // 3. Handle RLC response to UE creation
    // TODO

    // 4. Initiate MAC UE creation
    mac_ue_create_request_message ue_create_msg{};
    ue_create_msg.cell_index = resp.cell_index;
    ue_create_msg.ue_index   = resp.ue_index;
    ue_create_msg.crnti      = du_ctxt.ue_db[ue_create_msg.ue_index].crnti;
    du_ctxt.mac->ue_create_request(ue_create_msg);
  }

  void mac_ue_create_response(const mac_ue_create_request_response_message& resp)
  {
    // 5. Handle MAC response to UE creation request
    // TODO

    // 6. Signal F1AP the UE creation outcome
    du_ue_create_response_message f1ap_resp{};
    f1ap_resp.ue_index = resp.ue_index;
    du_ctxt.f1ap_notifier->on_du_ue_create_response(f1ap_resp);
  }

private:
  du_manager_context&  du_ctxt;
  du_ue_create_message ue_create_cmd;
};

} // namespace srsgnb

#endif // SRSGNB_UE_CREATION_PROCEDURE_H
