/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "sctp_network_gateway.h"
#include "srsgnb/asn1/f1ap.h"

namespace srsgnb {

sctp_network_gateway::sctp_network_gateway() : logger(srslog::fetch_basic_logger("SCTP-NW-GW")) {}

void sctp_network_gateway::handle_pdu(const byte_buffer& pdu)
{
  logger.info("Received PDU of {} bytes", pdu.length());

  // TODO: send over SCTP socket
}

} // namespace srsgnb
