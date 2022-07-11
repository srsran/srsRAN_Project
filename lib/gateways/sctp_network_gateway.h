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

#include "srsgnb/gateways/network_gateway.h"
#include "srsgnb/srslog/srslog.h"

namespace srsgnb {

class sctp_network_gateway : public network_gateway_data_handler
{
public:
  explicit sctp_network_gateway() : logger(srslog::fetch_basic_logger("SCTP-NW-GW")) {}

  void handle_pdu(const byte_buffer& pdu) override;

private:
  srslog::basic_logger& logger;
};

} // namespace srsgnb
