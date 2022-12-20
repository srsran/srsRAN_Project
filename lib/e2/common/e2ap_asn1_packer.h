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

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/e2/common/e2_common.h"
#include "srsgnb/gateways/network_gateway.h"
#include "srsgnb/srslog/srslog.h"
#include <cstdio>

namespace srsgnb {

class e2ap_asn1_packer : public e2_message_handler
{
public:
  explicit e2ap_asn1_packer(network_gateway_data_handler& gw, e2_message_handler& e2);

  void handle_packed_pdu(const byte_buffer& pdu);

  void handle_message(const e2_message& msg) override;

private:
  srslog::basic_logger&         logger;
  network_gateway_data_handler& gw;
  e2_message_handler&           e2;
};

} // namespace srsgnb
