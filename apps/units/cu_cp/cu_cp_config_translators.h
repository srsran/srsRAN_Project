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
