/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_ue_adapters.h"
#include "srsran/f1u/du/f1u_bearer.h"
#include "srsran/rlc/rlc_rx.h"
#include "srsran/rlc/rlc_tx.h"
#include "srsran/support/async/async_no_op_task.h"

using namespace srsran;
using namespace srs_du;

class null_sink_f1c_bearer : public f1c_bearer
{
public:
  void             handle_pdu(byte_buffer pdu) override {}
  async_task<bool> handle_pdu_and_await_delivery(byte_buffer               pdu,
                                                 bool                      report_rrc_delivery_status,
                                                 std::chrono::milliseconds timeout) override
  {
    return launch_no_op_task(true);
  }
  async_task<bool> handle_pdu_and_await_transmission(byte_buffer pdu, std::chrono::milliseconds timeout) override
  {
    return launch_no_op_task(true);
  }
  void handle_transmit_notification(uint32_t highest_pdcp_sn) override {}
  void handle_delivery_notification(uint32_t highest_pdcp_sn) override {}
  void handle_sdu(byte_buffer_chain sdu) override {}
} null_f1c_bearer;

/// \brief F1-U Bearer Sink. Used to discard events while in the process of destroying a UE bearer.
class null_sink_f1u_bearer : public f1u_bearer,
                             public f1u_rx_pdu_handler,
                             public f1u_tx_delivery_handler,
                             public f1u_tx_sdu_handler
{
public:
  f1u_rx_pdu_handler&      get_rx_pdu_handler() override { return *this; }
  f1u_tx_delivery_handler& get_tx_delivery_handler() override { return *this; }
  f1u_tx_sdu_handler&      get_tx_sdu_handler() override { return *this; }

  void handle_pdu(nru_dl_message msg) override {}
  void handle_transmit_notification(uint32_t highest_pdcp_sn, uint32_t desired_buf_size) override {}
  void handle_delivery_notification(uint32_t highest_pdcp_sn) override {}
  void handle_retransmit_notification(uint32_t highest_pdcp_sn) override {}
  void handle_delivery_retransmitted_notification(uint32_t highest_pdcp_sn) override {}
  void handle_sdu(byte_buffer_chain sdu) override {}
  void stop() override {}
} null_f1u_bearer;

class null_sink_rlc_bearer : public rlc_tx_upper_layer_data_interface, public rlc_rx_lower_layer_interface
{
public:
  void handle_sdu(byte_buffer sdu_buf, bool is_retx) override {}
  void discard_sdu(uint32_t pdcp_sn) override {}
  void handle_pdu(byte_buffer_slice pdu) override {}
} null_rlc_bearer;

void f1c_rx_sdu_rlc_adapter::disconnect()
{
  connect(null_rlc_bearer);
}

void f1u_gateway_nru_rx_adapter::disconnect()
{
  connect(null_f1u_bearer);
}

void f1u_rx_rlc_sdu_adapter::disconnect()
{
  connect(null_rlc_bearer);
}

void rlc_rx_rrc_sdu_adapter::disconnect()
{
  connect(null_f1c_bearer);
}

void rlc_f1u_tx_sdu_adapter::disconnect()
{
  connect(null_f1u_bearer);
}

void rlc_f1c_tx_data_notifier::disconnect()
{
  connect(null_f1c_bearer);
}

void rlc_f1u_tx_data_notifier::disconnect()
{
  connect(null_f1u_bearer);
}

void mac_sdu_rx_adapter::disconnect()
{
  connect(null_rlc_bearer);
}
