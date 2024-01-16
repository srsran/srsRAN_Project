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

#include "du_ue_adapters.h"
#include "srsran/f1u/du/f1u_bearer.h"
#include "srsran/rlc/rlc_rx.h"
#include "srsran/rlc/rlc_tx.h"

using namespace srsran;
using namespace srs_du;

class null_sink_f1c_bearer : public f1c_bearer
{
public:
  void handle_pdu(byte_buffer pdu) override {}
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
  void handle_transmit_notification(uint32_t highest_pdcp_sn) override {}
  void handle_delivery_notification(uint32_t highest_pdcp_sn) override {}
  void handle_sdu(byte_buffer_chain sdu) override {}
} null_f1u_bearer;

class null_sink_rlc_bearer : public rlc_tx_upper_layer_data_interface, public rlc_rx_lower_layer_interface
{
public:
  void handle_sdu(rlc_sdu sdu) override {}
  void discard_sdu(uint32_t pdcp_sn) override {}
  void handle_pdu(byte_buffer_slice pdu) override {}
} null_rlc_bearer;

void f1c_rx_sdu_rlc_adapter::disconnect()
{
  connect(null_rlc_bearer);
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
