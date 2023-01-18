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
#include "srsgnb/gateways/sctp_network_gateway.h"
#include "srsgnb/ngap/ngc.h"
#include "srsgnb/srslog/srslog.h"
#include <cstdio>

namespace srsgnb {

class ngc_asn1_packer : public srs_cu_cp::ngc_message_handler
{
public:
  explicit ngc_asn1_packer(sctp_network_gateway_data_handler& gw, ngc_message_handler& ngc);

  void handle_packed_pdu(const byte_buffer& pdu);

  void handle_message(const srs_cu_cp::ngc_message& msg) override;

private:
  srslog::basic_logger&              logger;
  sctp_network_gateway_data_handler& gw;
  ngc_message_handler&               ngc;
};

} // namespace srsgnb
