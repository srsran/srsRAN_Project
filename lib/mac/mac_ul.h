
#ifndef SRSGNB_MAC_UL_H
#define SRSGNB_MAC_UL_H

#include "../ran/gnb_format.h"
#include "mac_ctxt.h"
#include "mac_ul_demux.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/support/async/schedule_on.h"

namespace srsgnb {

class mac_ul_worker
{
public:
  explicit mac_ul_worker(mac_common_config_t& cfg_, mac_ul_sdu_notifier& ul_ccch_notifier_) :
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

  void remove_ue(du_ue_index_t ue_index) { demux.erase(ue_index); }

  void push_pdu(mac_rx_data_indication msg)
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
    for (const logical_channel_addmod& channel : ul_logical_channels) {
      ue->ul_bearers.insert(channel.lcid, channel.ul_bearer);
    }
    return true;
  }

  mac_common_config_t&  cfg;
  srslog::basic_logger& logger;

  mac_ul_demux demux;
};

class mac_ul
{
public:
  mac_ul(mac_common_config_t& cfg_, mac_ul_sdu_notifier& ul_ccch_notifier_) : cfg(cfg_)
  {
    workers.push_back(std::make_unique<mac_ul_worker>(cfg, ul_ccch_notifier_));
  }

  async_task<void> add_ue(const mac_ue_create_request_message& request)
  {
    // TODO: define dispatch policy to UL workers
    return dispatch_and_return_on(cfg.ul_exec, cfg.ctrl_exec, [this, request]() { workers[0]->add_ue(request); });
  }

  async_task<void> remove_ue(du_ue_index_t ue_index)
  {
    return dispatch_and_return_on(cfg.ul_exec, cfg.ctrl_exec, [this, ue_index]() { workers[0]->remove_ue(ue_index); });
  }

  void push_ul_pdu(mac_rx_data_indication pdu)
  {
    // Dispatch to right worker
    cfg.ul_exec.execute([this, pdu]() { workers[0]->push_pdu(std::move(pdu)); });
  }

  mac_ul_worker& get_worker() { return *workers[0]; }

private:
  mac_common_config_t&                         cfg;
  std::vector<std::unique_ptr<mac_ul_worker> > workers;
};

} // namespace srsgnb

#endif // SRSGNB_MAC_UL_H
