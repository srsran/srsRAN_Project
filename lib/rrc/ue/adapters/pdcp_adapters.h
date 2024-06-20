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

#include "srsran/pdcp/pdcp_rx.h"
#include "srsran/pdcp/pdcp_tx.h"
#include "srsran/ran/cause/ngap_cause.h"

namespace srsran {
namespace srs_cu_cp {

/// Adapter between PDCP Rx data and RRC in UL direction (Rx)
class pdcp_rrc_ue_rx_adapter : public pdcp_rx_upper_data_notifier, public pdcp_rx_upper_control_notifier
{
public:
  pdcp_rrc_ue_rx_adapter() = default;

  void on_new_sdu(byte_buffer sdu) override { rrc_pdus.push_back(std::move(sdu)); }

  void on_protocol_failure() override
  {
    srslog::fetch_basic_logger("PDCP").warning("Requesting UE release. Cause: Received protocol failure from PDCP Rx");
    cause = cause_protocol_t::unspecified;
  }

  void on_integrity_failure() override
  {
    srslog::fetch_basic_logger("PDCP").warning("Requesting UE release. Cause: Received integrity failure from PDCP Rx");
    cause = cause_protocol_t::unspecified;
  }

  void on_max_count_reached() override
  {
    srslog::fetch_basic_logger("PDCP").warning("Requesting UE release. Cause: Max count reached from PDCP Rx");
    cause = cause_protocol_t::unspecified;
  }

  std::variant<std::vector<byte_buffer>, ngap_cause_t> pop_result()
  {
    if (cause.has_value()) {
      auto ret = *cause;
      cause.reset();
      return ret;
    }
    return std::move(rrc_pdus);
  }

private:
  std::vector<byte_buffer>    rrc_pdus;
  std::optional<ngap_cause_t> cause;
};

/// Adapter between PDCP and RRC UE for DL PDUs
class pdcp_rrc_ue_tx_adapter : public pdcp_tx_lower_notifier
{
public:
  pdcp_rrc_ue_tx_adapter() = default;

  void on_new_pdu(byte_buffer pdu, bool is_retx) override
  {
    pdcp_pdu         = std::move(pdu);
    pdcp_pdu_is_retx = is_retx;
  }

  void on_discard_pdu(uint32_t pdcp_sn) override
  {
    // not implemented
  }

  byte_buffer get_pdcp_pdu() { return std::move(pdcp_pdu); }

private:
  byte_buffer pdcp_pdu;
  bool        pdcp_pdu_is_retx;
};

/// Adapter between PDCP Tx control and RRC
class pdcp_tx_control_rrc_ue_adapter : public pdcp_tx_upper_control_notifier
{
public:
  pdcp_tx_control_rrc_ue_adapter() = default;

  void on_protocol_failure() override
  {
    srslog::fetch_basic_logger("PDCP").warning("Requesting UE release. Cause: Received protocol failure from PDCP Tx");
    cause = cause_protocol_t::unspecified;
  }

  void on_max_count_reached() override
  {
    srslog::fetch_basic_logger("PDCP").warning("Requesting UE release. Cause: Max count reached from PDCP Tx");
    cause = cause_protocol_t::unspecified;
  }

  ngap_cause_t get_failure_cause() { return cause; }

private:
  ngap_cause_t cause;
};

} // namespace srs_cu_cp
} // namespace srsran
