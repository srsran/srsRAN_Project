
#ifndef SRSGNB_MAC_UL_UE_MANAGER_H
#define SRSGNB_MAC_UL_UE_MANAGER_H

#include "../../ran/gnb_format.h"
#include "../mac_config.h"
#include "mac_ul_demux.h"
#include "srsgnb/mac/mac.h"

namespace srsgnb {

class mac_ul_ue_manager
{
public:
  explicit mac_ul_ue_manager(mac_common_config_t& cfg_, mac_ul_sdu_notifier& ul_ccch_notifier_) :
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

  void handle_ul_sdu(mac_ul_sdu& sdu)
  {
    // Push PDU to upper layers
    du_ue_index_t        ue_idx;
    mac_ul_sdu_notifier* bearer;
    std::tie(ue_idx, bearer) = demux.find_ul_bearer(sdu.rnti, sdu.lcid);
    if (bearer == nullptr) {
      logger.warning("Received UL PDU for inexistent bearer {{" FMT_RNTI ", {}}}", sdu.rnti, sdu.lcid);
      return;
    }
    if (sdu.lcid == 0) {
      log_ul_pdu(logger, MAX_NOF_UES, sdu.rnti, MAX_NOF_CELLS, "CCCH", "Pushing {} bytes", sdu.pdu.size());
    } else {
      log_ul_pdu(
          logger, ue_idx, sdu.rnti, MAX_NOF_CELLS, "DCCH", "Pushing {} bytes to LCID={}", sdu.pdu.size(), sdu.lcid);
    }
    bearer->on_ul_sdu(sdu);
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

} // namespace srsgnb

#endif // SRSGNB_MAC_UL_UE_MANAGER_H
