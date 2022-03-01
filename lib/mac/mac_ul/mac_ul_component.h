
#ifndef SRSGNB_MAC_UL_COMPONENT_H
#define SRSGNB_MAC_UL_COMPONENT_H

#include "../../ran/gnb_format.h"
#include "../mac_config.h"
#include "../mac_config_interfaces.h"
#include "mac_ul_demux.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/support/async/execute_on.h"

namespace srsgnb {

class mac_pusch_processor
{
public:
  explicit mac_pusch_processor(mac_common_config_t& cfg_, mac_ul_sdu_notifier& ul_ccch_notifier_) :
    cfg(cfg_), logger(cfg.logger), demux(ul_ccch_notifier_)
  {}

  bool contains_ue(du_ue_index_t ue_index) const { return demux.contains_ue(ue_index); }

  bool add_ue(const mac_ue_create_request_message& request)
  {
    // 1. Insert UE
    if (not demux.insert(request.ue_index, request.crnti)) {
      log_proc_failure(logger, request.ue_index, request.crnti, "UE Create Request", "Failed to insert UE in DEMUX");
      return false;
    }

    // 2. Add UE Bearers
    if (not addmod_bearers(request.ue_index, request.bearers)) {
      log_proc_failure(
          logger, request.ue_index, request.crnti, "UE Create Request", "Failed to insert UL bearers in DEMUX");
      return false;
    }
    return true;
  }

  bool reconfigure_ue(const mac_ue_reconfiguration_request_message& request)
  {
    // 1. Remove UE bearers
    if (not remove_bearers(request.ue_index, request.bearers_to_rem)) {
      log_proc_failure(logger,
                       request.ue_index,
                       request.crnti,
                       "UE Reconfiguration Request",
                       "Failed to update UL bearers in DEMUX");
      return false;
    }

    // 2. Add/Mod UE Bearers
    if (not addmod_bearers(request.ue_index, request.bearers_to_addmod)) {
      log_proc_failure(logger,
                       request.ue_index,
                       request.crnti,
                       "UE Reconfiguration Request",
                       "Failed to update UL bearers in DEMUX");
      return false;
    }
    return true;
  }

  void remove_ue(du_ue_index_t ue_index) { demux.erase(ue_index); }

  void push_rx_data_indication(mac_rx_data_indication msg)
  {
    // Decode TB
    // TODO

    // Push PDU to upper layers
    du_ue_index_t        ue_idx;
    mac_ul_sdu_notifier* bearer;
    std::tie(ue_idx, bearer) = demux.find_ul_bearer(msg.rnti, msg.lcid);
    if (bearer == nullptr) {
      logger.warning("Received UL PDU for inexistent bearer {{" FMT_RNTI ", {}}}", msg.rnti, msg.lcid);
      return;
    }
    if (msg.lcid == 0) {
      log_ul_pdu(logger, msg.rnti, msg.cell_index, "CCCH", "Pushing {} bytes", msg.pdu.size());
    } else {
      log_ul_pdu(
          logger, ue_idx, msg.rnti, msg.cell_index, "DCCH", "Pushing {} bytes to LCID={}", msg.pdu.size(), msg.lcid);
    }
    bearer->on_ul_sdu(mac_ul_sdu{msg.rnti, msg.lcid, msg.pdu});
  }

private:
  bool addmod_bearers(du_ue_index_t ue_index, const std::vector<logical_channel_addmod>& ul_logical_channels)
  {
    mac_ul_ue* ue = demux.get_ue(ue_index);
    if (ue == nullptr) {
      return false;
    }
    for (const logical_channel_addmod& channel : ul_logical_channels) {
      ue->ul_bearers.insert(channel.lcid, channel.ul_bearer);
    }
    return true;
  }

  bool remove_bearers(du_ue_index_t ue_index, span<const lcid_t> lcids)
  {
    mac_ul_ue* ue = demux.get_ue(ue_index);
    if (ue == nullptr) {
      return false;
    }
    for (lcid_t lcid : lcids) {
      ue->ul_bearers.erase(lcid);
    }
    return true;
  }

  mac_common_config_t&  cfg;
  srslog::basic_logger& logger;

  mac_ul_demux demux;
};

class mac_ul_component final : public mac_ul_configurer
{
public:
  mac_ul_component(mac_common_config_t& cfg_, mac_ul_sdu_notifier& ul_ccch_notifier_) : cfg(cfg_)
  {
    workers.push_back(std::make_unique<mac_pusch_processor>(cfg, ul_ccch_notifier_));
  }

  async_task<bool> add_ue(const mac_ue_create_request_message& request) override
  {
    // TODO: define dispatch policy to UL workers
    return dispatch_and_resume_on(
        cfg.ul_exec, cfg.ctrl_exec, [this, request]() { return workers[0]->add_ue(request); });
  }

  async_task<bool> reconfigure_ue(const mac_ue_reconfiguration_request_message& request) override
  {
    return dispatch_and_resume_on(
        cfg.ul_exec, cfg.ctrl_exec, [this, request]() { return workers[0]->reconfigure_ue(request); });
  }

  async_task<void> remove_ue(const mac_ue_delete_request_message& msg) override
  {
    return dispatch_and_resume_on(
        cfg.ul_exec, cfg.ctrl_exec, [this, ue_index = msg.ue_index]() { workers[0]->remove_ue(ue_index); });
  }

  void push_rx_data_indication(mac_rx_data_indication msg)
  {
    // Dispatch to right worker
    cfg.ul_exec.execute([this, msg = std::move(msg)]() { workers[0]->push_rx_data_indication(std::move(msg)); });
  }

private:
  mac_common_config_t&                               cfg;
  std::vector<std::unique_ptr<mac_pusch_processor> > workers;
};

} // namespace srsgnb

#endif // SRSGNB_MAC_UL_COMPONENT_H
