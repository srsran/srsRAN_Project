
#ifndef SRSGNB_UE_CREATION_PROCEDURE_H
#define SRSGNB_UE_CREATION_PROCEDURE_H

#include "../ran/gnb_format.h"
#include "du_manager_context.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/rlc/rlc.h"
#include "srsgnb/rlc/rlc_factory.h"

namespace srsgnb {

class mac_ul_dcch_adapter : public mac_ul_sdu_notifier
{
public:
  mac_ul_dcch_adapter(rlc_ul_bearer& bearer_) : bearer(bearer_) {}
  void on_ul_sdu(mac_ul_sdu sdu) override { bearer.push_pdu(sdu.pdu); }

  rlc_ul_bearer& bearer;
};

class ue_creation_procedure
{
public:
  ue_creation_procedure(du_manager_context& du_ctxt_, const du_ue_create_message& ue_create_msg) :
    du_ctxt(du_ctxt_), ue_create_cmd(ue_create_msg), logger(srslog::fetch_basic_logger("DU-MNG"))
  {
    if (du_ctxt.ue_db.contains(ue_create_msg.ue_index)) {
      logger.error("Creating already existing ueId={}", ue_create_msg.ue_index);
      return;
    }
    log_proc_started(logger, ue_create_msg.ue_index, ue_create_msg.crnti, "UE Create");

    // 1. Construct future DU manager UE context object
    created_ue.pcell_index = ue_create_cmd.cell_index;
    created_ue.ue_index    = ue_create_cmd.ue_index;
    created_ue.crnti       = ue_create_cmd.crnti;
    created_ue.bearers.emplace(1);
    created_ue.bearers[1].ul_bearer       = create_rlc_ul_bearer(created_ue.ue_index, 1, *du_ctxt.rlc_ul_notifier);
    created_ue.bearers[1].mac_ul_notifier = std::make_unique<mac_ul_dcch_adapter>(*created_ue.bearers[1].ul_bearer);

    // 2. Initiate MAC UE creation
    mac_ue_create_request_message mac_ue_create_msg{};
    mac_ue_create_msg.cell_index = created_ue.pcell_index;
    mac_ue_create_msg.ue_index   = created_ue.ue_index;
    mac_ue_create_msg.crnti      = ue_create_cmd.crnti;
    for (du_ue_bearer_context& bearer : created_ue.bearers) {
      logical_channel_addmod lc;
      lc.lcid      = 1;
      lc.ul_bearer = bearer.mac_ul_notifier.get();
      mac_ue_create_msg.bearers.push_back(lc);
    }
    du_ctxt.mac->ue_create_request(mac_ue_create_msg);
  }

  void mac_ue_create_response(const mac_ue_create_request_response_message& resp)
  {
    // 4. Handle MAC response to UE creation request
    // TODO

    // 5. Register DU UE object in database
    du_ctxt.ue_db.insert(created_ue.ue_index, std::move(created_ue));
    log_proc_completed(logger, ue_create_cmd.ue_index, ue_create_cmd.crnti, "UE Create");

    // 6. Signal F1AP the UE creation outcome
    du_ue_create_response_message f1ap_resp{};
    f1ap_resp.ue_index = resp.ue_index;
    du_ctxt.f1ap_cfg_notifier->on_du_ue_create_response(f1ap_resp);
  }

private:
  du_manager_context&   du_ctxt;
  du_ue_create_message  ue_create_cmd;
  srslog::basic_logger& logger;

  du_ue_context created_ue;
};

} // namespace srsgnb

#endif // SRSGNB_UE_CREATION_PROCEDURE_H
