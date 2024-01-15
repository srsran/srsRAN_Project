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

#include "srsran/cu_cp/du_processor.h"
#include "srsran/f1ap/cu_cp/f1ap_cu.h"
#include "srsran/pdcp/pdcp_rx.h"
#include "srsran/pdcp/pdcp_tx.h"
#include "srsran/rrc/rrc.h"

namespace srsran {
namespace srs_cu_cp {

/// Adapter between PDCP Rx data and RRC in UL direction (Rx)
class pdcp_rrc_ue_rx_adapter : public pdcp_rx_upper_data_notifier
{
public:
  pdcp_rrc_ue_rx_adapter() = default;

  void on_new_sdu(byte_buffer sdu) override { rrc_pdu = std::move(sdu); }

  byte_buffer get_rrc_pdu() { return std::move(rrc_pdu); }

private:
  byte_buffer rrc_pdu;
};

/// Adapter between PDCP Rx control and RRC in UL direction (Rx)
class pdcp_rx_control_rrc_ue_adapter : public pdcp_rx_upper_control_notifier
{
public:
  explicit pdcp_rx_control_rrc_ue_adapter()
  {
    // TODO: connect a RRC handler
    srslog::fetch_basic_logger("PDCP").debug("No RRC handler for PDCP Rx control events. All events will be ignored.");
  }

  void on_protocol_failure() override
  {
    srslog::fetch_basic_logger("PDCP").warning("Ignoring on_protocol_failure() from PDCP Rx: No RRC handler.");
  }

  void on_integrity_failure() override
  {
    srslog::fetch_basic_logger("PDCP").warning("Ignoring on_integrity_failure() from PDCP Rx: No RRC handler.");
  }

  void on_max_count_reached() override
  {
    srslog::fetch_basic_logger("PDCP").warning("Ignoring on_max_count_reached() from PDCP Rx: No RRC handler.");
  }
};

/// Adapter between PDCP and RRC UE for DL PDUs
class pdcp_rrc_ue_tx_adapter : public pdcp_tx_lower_notifier
{
public:
  pdcp_rrc_ue_tx_adapter() = default;

  void on_new_pdu(pdcp_tx_pdu pdu) override { pdcp_pdu = std::move(pdu.buf); }

  void on_discard_pdu(uint32_t pdcp_sn) override
  {
    // not implemented
  }

  byte_buffer get_pdcp_pdu() { return std::move(pdcp_pdu); }

private:
  byte_buffer pdcp_pdu;
};

/// Adapter between PDCP Tx control and RRC
class pdcp_tx_control_rrc_ue_adapter : public pdcp_tx_upper_control_notifier
{
public:
  explicit pdcp_tx_control_rrc_ue_adapter()
  {
    // TODO: connect a RRC handler
    srslog::fetch_basic_logger("PDCP").debug("No RRC handler for PDCP Tx control events. All events will be ignored.");
  }

  void on_protocol_failure() override
  {
    srslog::fetch_basic_logger("PDCP").warning("Ignoring on_protocol_failure() from PDCP Tx: No RRC handler.");
  }

  void on_max_count_reached() override
  {
    srslog::fetch_basic_logger("PDCP").warning("Ignoring on_max_count_reached() from PDCP Tx: No RRC handler.");
  }
};

} // namespace srs_cu_cp
} // namespace srsran
