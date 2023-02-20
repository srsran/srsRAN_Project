/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/f1ap/common/f1ap_packet_procedures.h"
#include "srsgnb/f1ap/du/f1ap_du.h"
#include <cstdio>

namespace srsgnb {

// class f1ap_dl_packet_procedure : public f1ap_packet_procedures
//{
//   f1ap_du_pdu_notifier& listener;
//
// public:
//   explicit f1ap_dl_packet_procedure(f1ap_du_pdu_notifier& listener) : listener(listener) {}
//
//   void handle_ul_data_packet(byte_buffer& data) override
//   {
//     std::printf(
//         "[f1ap-dl-pkt-procedure] received a new data packet of size = %u bytes, performing ul packet procedure\n",
//         (unsigned)data.size());
//     std::printf("[f1ap-dl-pkt-procedure] ul packet procedure finished, delivering packet to upper layer\n");
//     listener.on_new_packet(data);
//   }
// };

} // namespace srsgnb
