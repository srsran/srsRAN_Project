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
#include "srsgnb/f1ap/du/f1ap_du.h"
#include "srsgnb/gateways/sctp_network_gateway.h"
#include "srsgnb/srslog/srslog.h"
#include <cstdio>

namespace srsran {

class f1ap_asn1_packer : public f1ap_message_handler
{
public:
  explicit f1ap_asn1_packer(sctp_network_gateway_data_handler& gw, f1ap_message_handler& f1ap);

  void handle_packed_pdu(const byte_buffer& pdu);

  void handle_message(const f1ap_message& msg) override;

private:
  srslog::basic_logger&              logger;
  sctp_network_gateway_data_handler& gw;
  f1ap_message_handler&              f1ap;
};

} // namespace srsran
