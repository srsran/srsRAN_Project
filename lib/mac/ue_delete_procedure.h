
#ifndef SRSGNB_UE_DELETE_PROCEDURE_H
#define SRSGNB_UE_DELETE_PROCEDURE_H

namespace srsgnb {

class mac_ue_delete_procedure
{
public:
  explicit mac_ue_delete_procedure(mac_context& ctxt_, const mac_ue_delete_request_message& req_) :
    ctxt(ctxt_), logger(srslog::fetch_basic_logger("MAC")), req(req_)
  {
    log_proc_started(logger, req.ue_index, req.crnti, "UE Delete Request");

    ctxt.cfg.dl_execs[req.cell_index]->execute([this]() {
      // 1. Remove sched UE
      ctxt.sched_itf.delete_ue_request(req.crnti);
    });
  }

  void sched_ue_delete_response()
  {
    // 2. Remove UE associated DL channels
    ctxt.dl_entities.erase(req.ue_index);

    ctxt.cfg.ul_exec.execute([this]() {
      // 3. Remove UE associated UL channels
      ctxt.ul_entities.erase(req.ue_index);

      // 4. Send back response to DU manager
      mac_ue_delete_response_message resp{};
      resp.crnti = req.crnti;
      ctxt.cfg.cfg_notifier.on_ue_delete_complete(resp);
    });
  }

private:
  mac_context&                  ctxt;
  srslog::basic_logger&         logger;
  mac_ue_delete_request_message req;
};

} // namespace srsgnb

#endif // SRSGNB_UE_DELETE_PROCEDURE_H
