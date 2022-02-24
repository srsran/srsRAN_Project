
#ifndef SRSGNB_UE_DELETE_PROCEDURE_H
#define SRSGNB_UE_DELETE_PROCEDURE_H

namespace srsgnb {

class mac_ue_delete_procedure final : public async_procedure<void>
{
public:
  explicit mac_ue_delete_procedure(const mac_ue_delete_request_message& msg,
                                   mac_common_config_t&                 cfg_,
                                   mac_ul&                              mac_ul_,
                                   mac_dl&                              mac_dl_) :
    req(msg), cfg(cfg_), ul_mac(mac_ul_), dl_mac(mac_dl_), logger(srslog::fetch_basic_logger("MAC"))
  {}

private:
  void start() override
  {
    log_proc_started(logger, req.ue_index, req.rnti, "UE Delete Request");

    // 1. Remove UE from MAC DL worker
    async_await(dl_mac.remove_ue(req), &mac_ue_delete_procedure::ue_dl_removed);
  }

  void ue_dl_removed()
  {
    // 2. Remove UE associated UL channels
    async_await(ul_mac.remove_ue(req.ue_index), &mac_ue_delete_procedure::ue_ul_removed);
  }

  void ue_ul_removed()
  {
    // 3. Send back response to DU manager
    mac_ue_delete_response_message resp{};
    resp.ue_index = req.ue_index;
    cfg.cfg_notifier.on_ue_delete_complete(resp);
  }

private:
  mac_ue_delete_request_message req;
  mac_common_config_t&          cfg;
  mac_ul&                       ul_mac;
  mac_dl&                       dl_mac;
  srslog::basic_logger&         logger;
};

} // namespace srsgnb

#endif // SRSGNB_UE_DELETE_PROCEDURE_H
