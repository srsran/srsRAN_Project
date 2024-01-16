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

#include "srsran/cu_cp/du_repository.h"
#include "srsran/f1ap/du/f1c_connection_client.h"

namespace srsran {

class dlt_pcap;

/// Implementation of a DU and CU-CP F1-C gateway for the case that the DU and CU-CP are co-located.
class f1c_gateway_local_connector final : public srs_du::f1c_connection_client
{
public:
  explicit f1c_gateway_local_connector(dlt_pcap& f1ap_pcap_writer_);

  void attach_cu_cp(srs_cu_cp::du_repository& cu_cp_du_mng_);

  // DU interface.
  std::unique_ptr<f1ap_message_notifier>
  handle_du_connection_request(std::unique_ptr<f1ap_message_notifier> du_rx_pdu_notifier) override;

private:
  dlt_pcap&                 f1ap_pcap_writer;
  srs_cu_cp::du_repository* cu_cp_du_mng = nullptr;
};

} // namespace srsran
