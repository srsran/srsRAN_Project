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

#include "lib/ngap/ngap_asn1_packer.h"
#include "srsran/ngap/ngap.h"
#include "srsran/support/io/io_broker.h"

namespace srsran {
namespace srs_cu_cp {

/// Handler of NG connection between CU-CP and AMF.
class ngap_gateway_connector : public ngap_message_notifier
{
public:
  virtual void connect_cu_cp(ngap_message_handler& msg_handler_, ngap_event_handler& ev_handler_) = 0;

  virtual void disconnect() = 0;
};

/// Parameters for the NG gateway instantiation.
struct ngap_gateway_params {
  /// Parameters for a local AMF stub connection.
  struct no_core {};

  /// Parameters specific to an SCTP network gateway.
  struct network {
    io_broker&                          broker;
    srsran::sctp_network_gateway_config sctp;
  };

  /// PCAP writer for the NGAP messages.
  dlt_pcap& pcap;

  /// Mode of operation.
  variant<no_core, network> mode;
};

std::unique_ptr<ngap_gateway_connector> create_ngap_gateway(const ngap_gateway_params& params);

} // namespace srs_cu_cp
} // namespace srsran
