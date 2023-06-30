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

#include "srsran/cu_cp/du_processor.h"
#include "srsran/f1ap/cu_cp/f1ap_cu.h"
#include "srsran/pdcp/pdcp_rx.h"
#include "srsran/pdcp/pdcp_tx.h"
#include "srsran/rrc/rrc.h"

namespace srsran {
namespace srs_cu_cp {

/// Adapter between PDCP Rx data and RRC in UL direction (Rx)
class pdcp_rrc_ue_adapter : public pdcp_rx_upper_data_notifier
{
public:
  explicit pdcp_rrc_ue_adapter(rrc_ul_dcch_pdu_handler& rrc_rx) : rrc_handler(rrc_rx) {}

  void on_new_sdu(byte_buffer sdu) override { rrc_handler.handle_ul_dcch_pdu(std::move(sdu)); }

private:
  rrc_ul_dcch_pdu_handler& rrc_handler;
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

/// Adapter between PDCP and DU processor for DL PDUs
class pdcp_du_processor_adapter : public pdcp_tx_lower_notifier
{
public:
  explicit pdcp_du_processor_adapter(f1ap_rrc_message_handler& f1ap_handler_,
                                     const ue_index_t&         ue_index_,
                                     const srb_id_t&           srb_id_,
                                     const ue_index_t&         old_ue_index_ = ue_index_t::invalid) :
    f1ap_handler(f1ap_handler_), ue_index(ue_index_), srb_id(srb_id_), old_ue_index(old_ue_index_)
  {
  }

  void on_new_pdu(pdcp_tx_pdu pdu) override
  {
    f1ap_dl_rrc_message f1ap_msg = {};
    f1ap_msg.ue_index            = ue_index;
    f1ap_msg.old_ue_index        = old_ue_index;
    f1ap_msg.srb_id              = srb_id;

    f1ap_msg.rrc_container.resize(pdu.buf.length());
    std::copy(pdu.buf.begin(), pdu.buf.end(), f1ap_msg.rrc_container.begin());
    f1ap_handler.handle_dl_rrc_message_transfer(f1ap_msg);

    // reset old ue index after transmission
    old_ue_index = ue_index_t::invalid;
  }

  void on_discard_pdu(uint32_t pdcp_sn) override
  {
    // not implemented
  }

private:
  f1ap_rrc_message_handler& f1ap_handler;
  const ue_index_t          ue_index;
  const srb_id_t            srb_id;
  ue_index_t                old_ue_index;
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
