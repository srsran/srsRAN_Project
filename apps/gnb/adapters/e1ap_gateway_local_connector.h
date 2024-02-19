/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/cu_cp/cu_cp_e1_handler.h"
#include "srsran/e1ap/cu_up/e1ap_connection_client.h"

namespace srsran {

class dlt_pcap;

/// Implementation of a CU-UP and CU-CP E1AP gateway for the case that the CU-UP and CU-CP are co-located.
class e1ap_gateway_local_connector final : public srs_cu_up::e1ap_connection_client
{
public:
  explicit e1ap_gateway_local_connector(dlt_pcap& e1ap_pcap_writer_);

  void attach_cu_cp(srs_cu_cp::cu_cp_e1_handler& cu_cp_du_mng_);

  // CU-UP interface.
  std::unique_ptr<e1ap_message_notifier>
  handle_cu_up_connection_request(std::unique_ptr<e1ap_message_notifier> cu_up_rx_pdu_notifier) override;

private:
  dlt_pcap&                    e1ap_pcap_writer;
  srs_cu_cp::cu_cp_e1_handler* cu_cp_cu_up_mng = nullptr;
};

} // namespace srsran
