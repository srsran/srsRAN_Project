/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "srsran/f1ap/common/f1ap_packet_procedures.h"
#include "srsran/f1ap/du/f1ap_du.h"
#include <cstdio>

namespace srsran {

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

} // namespace srsran
