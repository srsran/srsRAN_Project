/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_LIB_DU_HIGH_ADAPTERS_H
#define SRSGNB_LIB_DU_HIGH_ADAPTERS_H

#include "srsgnb/du_manager/du_manager.h"
#include "srsgnb/f1_interface/du/f1ap_du.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/rlc/rlc.h"
#include "srsgnb/srslog/srslog.h"

namespace srsgnb {
namespace srs_du {

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

class du_manager_mac_event_indicator : public mac_ul_ccch_notifier
{
public:
  void connect(du_manager_ccch_handler& du_mng_) { du_mng = &du_mng_; }
  void on_ul_ccch_msg_received(const ul_ccch_indication_message& msg) override
  {
    du_mng->handle_ul_ccch_indication(msg);
  }

private:
  du_manager_ccch_handler* du_mng;
};

} // namespace srs_du
} // namespace srsgnb

#endif // SRSGNB_LIB_DU_HIGH_ADAPTERS_H
