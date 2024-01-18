/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "dummy_cu_up.h"
#include "srsran/e1ap/common/e1ap_common.h"
#include "srsran/e1ap/common/e1ap_message.h"

using namespace srsran;
using namespace srs_cu_cp;

void synchronized_dummy_cu_up::handle_rx_pdu(const e1ap_message& pdu)
{
  report_fatal_error_if_not(rx_pdus.push_blocking(pdu), "queue is full");
}

bool synchronized_dummy_cu_up::request_new_cu_up_connection()
{
  class sink_e1ap_message_notifier : public e1ap_message_notifier
  {
  public:
    sink_e1ap_message_notifier(synchronized_dummy_cu_up& cu_up_) : cu_up(cu_up_) {}
    void on_new_message(const e1ap_message& msg) override { cu_up.handle_rx_pdu(msg); }

  private:
    synchronized_dummy_cu_up& cu_up;
  };

  // Create notifier that will save the PDUs sent by the CU-CP.
  auto cu_cp_tx_notifier = std::make_unique<sink_e1ap_message_notifier>(*this);

  // Start CU-UP connection to CU-CP.
  tx_pdu_notifier = cu_cp_node->handle_new_cu_up_connection(std::move(cu_cp_tx_notifier));
  return tx_pdu_notifier != nullptr;
}

bool synchronized_dummy_cu_up::try_pop_rx_pdu(e1ap_message& pdu)
{
  return rx_pdus.try_pop(pdu);
}
