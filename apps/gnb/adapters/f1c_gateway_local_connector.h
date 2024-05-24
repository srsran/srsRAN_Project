/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/cu_cp/cu_cp_f1c_handler.h"
#include "srsran/f1ap/gateways/f1c_connection_client.h"

namespace srsran {

class dlt_pcap;

/// Implementation of a DU and CU-CP F1-C gateway for the case that the DU and CU-CP are co-located.
class f1c_gateway_local_connector final : public srs_du::f1c_connection_client
{
public:
  explicit f1c_gateway_local_connector(dlt_pcap& f1ap_pcap_writer_);

  void attach_cu_cp(srs_cu_cp::cu_cp_f1c_handler& cu_cp_du_mng_);

  // DU interface.
  std::unique_ptr<f1ap_message_notifier>
  handle_du_connection_request(std::unique_ptr<f1ap_message_notifier> du_rx_pdu_notifier) override;

private:
  dlt_pcap&                     f1ap_pcap_writer;
  srs_cu_cp::cu_cp_f1c_handler* cu_cp_du_mng = nullptr;
};

} // namespace srsran
