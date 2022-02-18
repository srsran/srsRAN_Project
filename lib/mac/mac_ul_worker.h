
#ifndef SRSGNB_MAC_UL_H
#define SRSGNB_MAC_UL_H

#include "../ran/gnb_format.h"
#include "demux.h"
#include "srsgnb/mac/mac.h"

namespace srsgnb {

class mac_ul_worker
{
public:
  explicit mac_ul_worker(mac_ul_sdu_notifier& ul_ccch_notifier_) :
    logger(srslog::fetch_basic_logger("MAC")), demux(ul_ccch_notifier_)
  {}

  bool add_ue(const mac_ue_create_request_message& request)
  {
    // 1. Insert UE
    if (not demux.insert(request.ue_index, request.crnti)) {
      log_proc_failure(logger, request.ue_index, request.crnti, "UE Create Request", "Failed to insert UE in DEMUX");
      return false;
    }

    // 2. Add UE Bearers
    if (not addmod_bearers(request.ue_index, request.ul_bearers)) {
      log_proc_failure(
          logger, request.ue_index, request.crnti, "UE Create Request", "Failed to insert UL bearers in DEMUX");
      return false;
    }
    return true;
  }

  void remove_ue(du_ue_index_t ue_index) { demux.erase(ue_index); }

  void push_pdu(rnti_t rnti, lcid_t lcid, du_cell_index_t cell_index, byte_buffer pdu)
  {
    // Decode TB
    // TODO

    // Push PDU to upper layers
    du_ue_index_t        ue_idx;
    mac_ul_sdu_notifier* bearer;
    std::tie(ue_idx, bearer) = demux.find_ul_bearer(rnti, lcid);
    if (bearer == nullptr) {
      logger.warning("Received UL PDU for inexistent bearer {{" FMT_RNTI ", {}}}", rnti, lcid);
      return;
    }
    if (lcid == 0) {
      log_ul_pdu(logger, rnti, cell_index, "CCCH", "Pushing {} bytes", pdu.size());
    } else {
      log_ul_pdu(logger, ue_idx, rnti, cell_index, "DCCH", "Pushing {} bytes to LCID={}", pdu.size(), lcid);
    }
    bearer->on_ul_sdu(mac_ul_sdu{rnti, lcid, pdu});
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

  srslog::basic_logger& logger;

  mac_ul_demux demux;
};

} // namespace srsgnb

#endif // SRSGNB_MAC_UL_H
