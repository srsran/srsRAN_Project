/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include <string>

namespace srsgnb {

/// \brief GTP-Tunnel Identifier.
struct gtp_tunnel {
  std::string transport_layer_address;
  uint32_t    gtp_teid;
};

} // namespace srsgnb