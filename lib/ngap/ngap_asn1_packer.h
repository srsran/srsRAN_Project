/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "srsran/adt/byte_buffer.h"
#include "srsran/gateways/sctp_network_gateway.h"
#include "srsran/ngap/ngap.h"
#include "srsran/pcap/dlt_pcap.h"
#include "srsran/srslog/srslog.h"
#include <cstdio>

namespace srsran {
namespace srs_cu_cp {

struct ngap_message;

class ngap_asn1_packer : public srs_cu_cp::ngap_message_handler
{
public:
  ngap_asn1_packer(sctp_network_gateway_data_handler& gw, ngap_message_handler& ngap, dlt_pcap& pcap_);

  void handle_packed_pdu(const byte_buffer& pdu);

  void handle_message(const srs_cu_cp::ngap_message& msg) override;

private:
  srslog::basic_logger&              logger;
  sctp_network_gateway_data_handler& gw;
  ngap_message_handler&              ngap;
  dlt_pcap&                          pcap;
};

} // namespace srs_cu_cp
} // namespace srsran
