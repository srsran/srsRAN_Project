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

#include "srsran/cu_cp/cu_cp_configuration.h"
#include "srsran/ngap/gateways/n2_connection_client_factory.h"

namespace srsran {

struct cu_cp_unit_config;
struct cu_cp_unit_amf_config;

/// Converts and returns the given gnb application configuration to a CU-CP configuration.
srs_cu_cp::cu_cp_configuration generate_cu_cp_config(const cu_cp_unit_config& cu_cfg);

/// Converts CU-CP configuration into N2 connection client.
srs_cu_cp::n2_connection_client_config
generate_n2_client_config(const cu_cp_unit_amf_config& amf_cfg, dlt_pcap& pcap_writer, io_broker& broker);

} // namespace srsran
