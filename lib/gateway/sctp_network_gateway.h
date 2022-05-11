/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_SCTP_NETWORK_GATEWAY_H
#define SRSGNB_SCTP_NETWORK_GATEWAY_H

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/gateways/network_gateway.h"
#include "srsgnb/srslog/srslog.h"
#include <cstdio>

namespace srsgnb {

class sctp_network_gateway : public network_gateway_data_handler
{
public:
  explicit sctp_network_gateway();
  void handle_pdu(const byte_buffer& pdu) override;

private:
  srslog::basic_logger& logger;
};

} // namespace srsgnb

#endif // SRSGNB_SCTP_NETWORK_GATEWAY_H
