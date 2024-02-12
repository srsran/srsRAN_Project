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
