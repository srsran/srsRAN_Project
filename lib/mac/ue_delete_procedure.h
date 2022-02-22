
#ifndef SRSGNB_UE_DELETE_PROCEDURE_H
#define SRSGNB_UE_DELETE_PROCEDURE_H

namespace srsgnb {

class mac_ue_delete_procedure final : public async_procedure<void>
{
public:
  explicit mac_ue_delete_procedure(mac_context& ctxt_, mac_ue_context& ue_ctx_, manual_event_flag& sched_ue_deleted_) :
    ctxt(ctxt_), ue_ctx(ue_ctx_), logger(srslog::fetch_basic_logger("MAC")), sched_ue_deleted(sched_ue_deleted_)
  {}

  void start() override
  {
    log_proc_started(logger, ue_ctx.du_ue_index, ue_ctx.rnti, "UE Delete Request");

    async_await(sched_ue_deleted, &mac_ue_delete_procedure::sched_ue_delete_response);
    ctxt.cfg.dl_execs[ue_ctx.pcell_idx]->execute([this]() {
      // 1. Remove UE from MAC DL worker
      ctxt.dl_worker.remove_ue(ue_ctx.du_ue_index);
    });
  }

  void sched_ue_delete_response()
  {
    // 2. Remove UE associated DL channels
    ctxt.dl_worker.on_sched_ue_remove_complete(ue_ctx.du_ue_index);

    ctxt.cfg.ul_exec.execute([this]() {
      // 3. Remove UE associated UL channels
      ctxt.ul_worker.remove_ue(ue_ctx.du_ue_index);

      // 4. Send back response to DU manager
      mac_ue_delete_response_message resp{};
      resp.ue_index = ue_ctx.du_ue_index;
      ctxt.cfg.cfg_notifier.on_ue_delete_complete(resp);
    });
  }

private:
  mac_context&          ctxt;
  mac_ue_context&          ue_ctx;
  srslog::basic_logger& logger;
  manual_event_flag&    sched_ue_deleted;
};

} // namespace srsgnb

#endif // SRSGNB_UE_DELETE_PROCEDURE_H
