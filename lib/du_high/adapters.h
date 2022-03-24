
#ifndef SRSGNB_ADAPTERS_H
#define SRSGNB_ADAPTERS_H

#include "srsgnb/du_manager/du_manager.h"
#include "srsgnb/f1_interface/f1ap_du.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/rlc/rlc.h"
#include "srsgnb/srslog/srslog.h"

namespace srsgnb {

// class du_ccch_pdu_handler : public ul_ccch_pdu_notifier
//{
// public:
//   void handle_pdu(const byte_buffer& pdu) override
//   {
//     printf("[RLC-DUMANAGER-ADAPTER] Received a packet from F1u layer, forwarding data packet of size = %u bytes to "
//            "upper layer (PDCP)\n",
//            (unsigned)pdu.size());
//     du_manager.ul_ccch_indication(pdu);
//   }
//
// private:
//   du_manager_input_gateway& du_manager;
// };

class du_manager_mac_event_indicator : public mac_event_notifier
{
public:
  void connect(du_manager_event_handler& du_mng_) { du_mng = &du_mng_; }
  void on_ul_ccch_msg_received(const ul_ccch_indication_message& msg) override { du_mng->handle_ul_ccch_indication(msg); }

private:
  du_manager_event_handler* du_mng;
};

class rlc_ul_sdu_adapter : public rlc_sdu_rx_notifier
{
public:
  void connect(f1ap_du_ul_interface& f1ap_) { f1ap = &f1ap_; }

  void on_new_sdu(du_ue_index_t ue_index, lcid_t lcid, byte_buffer pdu) override
  {
    if (is_srb(lcid)) {
      f1_rx_pdu msg{};
      msg.ue_index = ue_index;
      msg.lcid     = lcid;
      msg.pdu      = std::move(pdu);
      f1ap->handle_pdu(msg);
    } else {
      // TODO
    }
  }

private:
  f1ap_du_ul_interface* f1ap = nullptr;
};

class f1ap_du_rlc_connector : public f1ap_du_pdu_notifier
{
  void push_pdu(const byte_buffer& data) override {}
};

} // namespace srsgnb

#endif // SRSGNB_ADAPTERS_H
