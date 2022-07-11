/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/rrc/rrc.h"

namespace srsgnb {
namespace srs_cu_cp {

/// Adapter between UE manager and RRC
class rrc_ul_ccch_adapter : public srb_pdu_notifier
{
public:
  explicit rrc_ul_ccch_adapter(rrc_ul_ccch_pdu_handler& rrc_rx) : rrc_handler(rrc_rx) {}

  void on_new_pdu(byte_buffer_slice pdu) override { rrc_handler.handle_ul_ccch_pdu(std::move(pdu)); }

  void on_new_rrc_message(asn1::unbounded_octstring<true> rrc_container) override
  {
    byte_buffer_slice pdu({rrc_container.begin(), rrc_container.end()});
    rrc_handler.handle_ul_ccch_pdu(std::move(pdu));
  }

private:
  rrc_ul_ccch_pdu_handler& rrc_handler;
};

} // namespace srs_cu_cp
} // namespace srsgnb
