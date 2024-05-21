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

#include "srsran/cu_cp/cu_cp_f1c_handler.h"
#include "srsran/f1ap/du/f1c_connection_client.h"
#include "srsran/gateways/sctp_network_gateway.h"

namespace srsran {

class dlt_pcap;
class io_broker;

struct f1c_gateway_params {
  /// PCAP writer for the F1AP messages.
  dlt_pcap&                           pcap;
  io_broker&                          broker;
  srsran::sctp_network_gateway_config sctp;
};

/// \brief Create an F1-C gateway connector that the DU can use to connect to the CU-CP.
std::unique_ptr<srs_du::f1c_connection_client> create_f1c_gateway_connector(const f1c_gateway_params& params);

class f1c_connection_server
{
public:
  virtual ~f1c_connection_server() = default;

  virtual void attach_cu_cp(srs_cu_cp::cu_cp_f1c_handler& cu_cp_du_mng_) = 0;
};

struct f1c_cu_gateway_params {
  dlt_pcap&                           pcap;
  io_broker&                          broker;
  srsran::sctp_network_gateway_config sctp;
};

std::unique_ptr<f1c_connection_server> create_f1c_gateway_connector(const f1c_cu_gateway_params& params);

} // namespace srsran
