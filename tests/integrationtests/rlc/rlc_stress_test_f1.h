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

#include "lib/rlc/rlc_bearer_logger.h"
#include "srsran/pdcp/pdcp_rx.h"
#include "srsran/pdcp/pdcp_tx.h"
#include "srsran/rlc/rlc_rx.h"
#include "srsran/rlc/rlc_tx.h"
#include <random>

namespace srsran {
class f1ap_dummy : public pdcp_tx_lower_notifier,
                   public rlc_tx_upper_layer_data_notifier,
                   public rlc_tx_upper_layer_control_notifier,
                   public rlc_rx_upper_layer_data_notifier
{
  rlc_bearer_logger logger;

  rlc_tx_upper_layer_data_interface* rlc_tx_upper  = nullptr;
  pdcp_rx_lower_interface*           pdcp_rx_lower = nullptr;

public:
  f1ap_dummy(uint32_t id) : logger("F1AP", {0, id, drb_id_t::drb1, "DL"}) {}

  // PDCP -> F1 -> RLC
  void on_new_pdu(pdcp_tx_pdu pdu) final
  {
    rlc_sdu sdu = {};
    sdu.buf     = std::move(pdu.buf);
    sdu.pdcp_sn = pdu.pdcp_sn;
    logger.log_info("Passing F1AP PDU to RLC");
    rlc_tx_upper->handle_sdu(std::move(sdu));
  }

  // PDCP -> F1AP -> RLC
  void on_discard_pdu(uint32_t pdcp_sn) final
  {
    logger.log_debug("Discard PDU called");
    // TODO
  }

  // RLC -> F1AP -> PDCP
  void on_transmitted_sdu(uint32_t max_tx_pdcp_sn) final
  {
    logger.log_debug("Transmitted SDU called");
    // TODO
  }

  // RLC -> F1AP -> PDCP
  void on_delivered_sdu(uint32_t max_deliv_pdcp_sn) final
  {
    logger.log_debug("Delivered SDU called");
    // TODO
  }

  // RLC -> F1AP -> PDCP
  void on_new_sdu(byte_buffer_chain pdu) final
  {
    logger.log_debug("Passing SDU to PDCP");
    // TODO for now we copy to a new byte buffer
    byte_buffer buf;
    for (uint8_t byte : pdu) {
      report_error_if_not(buf.append(byte), "Failed to allocate byte buffer");
    }
    pdcp_rx_lower->handle_pdu(byte_buffer_chain{std::move(buf)});
  }

  // RLC -> F1 -> RRC
  void on_protocol_failure() final {}
  void on_max_retx() final {}

  void set_rlc_tx_upper_data(rlc_tx_upper_layer_data_interface* rlc_tx_upper_) { this->rlc_tx_upper = rlc_tx_upper_; }

  void set_pdcp_rx_lower(pdcp_rx_lower_interface* pdcp_rx_lower_) { this->pdcp_rx_lower = pdcp_rx_lower_; }
};

} // namespace srsran
