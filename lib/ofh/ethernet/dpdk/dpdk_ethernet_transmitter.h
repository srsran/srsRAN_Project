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

#include "srsran/ofh/ethernet/ethernet_gateway.h"
#include "srsran/srslog/logger.h"

struct rte_mempool;

namespace srsran {
namespace ether {

struct gw_config;

/// DPDK Ethernet transmitter implementation.
class dpdk_transmitter_impl : public gateway
{
public:
  explicit dpdk_transmitter_impl(const gw_config& config, srslog::basic_logger& logger_);

  // See interface for documentation.
  void send(span<span<const uint8_t>> frames) override;

private:
  srslog::basic_logger& logger;
  const unsigned        port_id   = 0;
  ::rte_mempool*        mbuf_pool = nullptr;
};

} // namespace ether
} // namespace srsran
