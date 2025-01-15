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

#include "srsran/ofh/ethernet/dpdk/dpdk_ethernet_port_context.h"
#include "srsran/ofh/ethernet/ethernet_gateway.h"
#include "srsran/srslog/logger.h"

namespace srsran {
namespace ether {

struct gw_config;

/// DPDK Ethernet transmitter implementation.
class dpdk_transmitter_impl : public gateway
{
public:
  dpdk_transmitter_impl(std::shared_ptr<dpdk_port_context> port_ctx_, srslog::basic_logger& logger_) :
    logger(logger_), port_ctx(std::move(port_ctx_))
  {
    srsran_assert(port_ctx, "Invalid port context");
  }

  // See interface for documentation.
  void send(span<span<const uint8_t>> frames) override;

private:
  srslog::basic_logger&              logger;
  std::shared_ptr<dpdk_port_context> port_ctx;
};

} // namespace ether
} // namespace srsran
