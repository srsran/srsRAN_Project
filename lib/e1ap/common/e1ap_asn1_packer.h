/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/e1ap/common/e1ap_common.h"
#include "srsgnb/gateways/sctp_network_gateway.h"
#include "srsgnb/srslog/srslog.h"
#include <cstdio>

namespace srsran {

class e1ap_asn1_packer : public e1ap_message_handler
{
public:
  explicit e1ap_asn1_packer(sctp_network_gateway_data_handler& gw, e1ap_message_handler& e1ap);

  void handle_packed_pdu(const byte_buffer& pdu);

  void handle_message(const e1ap_message& msg) override;

private:
  srslog::basic_logger&              logger;
  sctp_network_gateway_data_handler& gw;
  e1ap_message_handler&              e1ap;
};

} // namespace srsran
