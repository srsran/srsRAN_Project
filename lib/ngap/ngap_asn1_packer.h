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
#include "srsgnb/gateways/sctp_network_gateway.h"
#include "srsgnb/ngap/ngap.h"
#include "srsgnb/pcap/pcap.h"
#include "srsgnb/srslog/srslog.h"
#include <cstdio>

namespace srsran {

class ngap_asn1_packer : public srs_cu_cp::ngap_message_handler
{
public:
  ngap_asn1_packer(sctp_network_gateway_data_handler& gw, ngap_message_handler& ngap, ngap_pcap& pcap_);

  void handle_packed_pdu(const byte_buffer& pdu);

  void handle_message(const srs_cu_cp::ngap_message& msg) override;

private:
  srslog::basic_logger&              logger;
  sctp_network_gateway_data_handler& gw;
  ngap_message_handler&              ngap;
  ngap_pcap&                         pcap;
};

} // namespace srsran
