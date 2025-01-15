/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/ofh/ethernet/ethernet_gateway.h"
#include "srsran/ofh/ethernet/ethernet_gw_config.h"
#include "srsran/srslog/logger.h"
#include <linux/if_packet.h>

namespace srsran {
namespace ether {

/// Implementation for the Ethernet transmitter.
class transmitter_impl : public gateway
{
public:
  transmitter_impl(const gw_config& config, srslog::basic_logger& logger_);
  ~transmitter_impl() override;

  // See interface for documentation.
  void send(span<span<const uint8_t>> frames) override;

private:
  srslog::basic_logger& logger;
  int                   socket_fd = -1;
  ::sockaddr_ll         socket_address;
};

} // namespace ether
} // namespace srsran
