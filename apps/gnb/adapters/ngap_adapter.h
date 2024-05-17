/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
    io_broker&                            broker;
    srsran::sctp_network_connector_config sctp;
  };

  /// PCAP writer for the NGAP messages.
  dlt_pcap& pcap;

  /// Mode of operation.
  variant<no_core, network> mode;
};

std::unique_ptr<ngap_gateway_connector> create_ngap_gateway(const ngap_gateway_params& params);

} // namespace srs_cu_cp
} // namespace srsran
