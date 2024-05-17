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

#include "srsran/cu_up/cu_up_types.h"
#include "srsran/gtpu/gtpu_config.h"
#include "srsran/gtpu/gtpu_tunnel_common_tx.h"
#include "srsran/gtpu/gtpu_tunnel_nru.h"
#include "srsran/gtpu/gtpu_tunnel_nru_rx.h"
#include "srsran/pcap/dlt_pcap.h"
#include <memory>

/// This factory header file depends on the GTP-U tunnel interfaces (see above include list). It is kept separate as
/// clients of the GTP-U tunnel interfaces do not need to call factory methods.
namespace srsran {

struct gtpu_tunnel_nru_creation_message {
  srs_cu_up::ue_index_t                       ue_index;
  gtpu_tunnel_nru_config                      cfg;
  dlt_pcap*                                   gtpu_pcap;
  gtpu_tunnel_nru_rx_lower_layer_notifier*    rx_lower;
  gtpu_tunnel_common_tx_upper_layer_notifier* tx_upper;
};

/// Creates an instance of a GTP-U entity.
std::unique_ptr<gtpu_tunnel_nru> create_gtpu_tunnel_nru(gtpu_tunnel_nru_creation_message& msg);

} // namespace srsran