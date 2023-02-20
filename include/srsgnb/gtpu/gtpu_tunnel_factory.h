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

#include "srsgnb/gtpu/gtpu_config.h"
#include "srsgnb/gtpu/gtpu_tunnel.h"
#include <memory>

/// This factory header file depends on the GTP-U tunnel interfaces (see above include list). It is kept separate as
/// clients of the GTP-U tunnel interfaces do not need to call factory methods.
namespace srsran {

struct gtpu_tunnel_creation_message {
  uint32_t                             ue_index;
  gtpu_config                          cfg;
  gtpu_tunnel_rx_lower_layer_notifier* rx_lower;
  gtpu_tunnel_tx_upper_layer_notifier* tx_upper;
};

/// Creates an instance of a GTP-U entity.
std::unique_ptr<gtpu_tunnel> create_gtpu_tunnel(gtpu_tunnel_creation_message& msg);

} // namespace srsran
