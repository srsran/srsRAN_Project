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

#include "../ethernet_tx_metrics_collector_impl.h"
#include "srsran/ofh/ethernet/dpdk/dpdk_ethernet_port_context.h"
#include "srsran/ofh/ethernet/ethernet_transmitter.h"
#include "srsran/srslog/logger.h"

namespace srsran {
namespace ether {

struct transmitter_config;

/// DPDK Ethernet transmitter implementation.
class dpdk_transmitter_impl : public transmitter
{
public:
  dpdk_transmitter_impl(std::shared_ptr<dpdk_port_context> port_ctx_,
                        srslog::basic_logger&              logger_,
                        bool                               are_metrics_enabled) :
    logger(logger_), port_ctx(std::move(port_ctx_)), metrics_collector(are_metrics_enabled)
  {
    srsran_assert(port_ctx, "Invalid port context");
  }

  // See interface for documentation.
  void send(span<span<const uint8_t>> frames) override;

  // See interface for documentation.
  transmitter_metrics_collector* get_metrics_collector() override;

private:
  srslog::basic_logger&              logger;
  std::shared_ptr<dpdk_port_context> port_ctx;
  transmitter_metrics_collector_impl metrics_collector;
};

} // namespace ether
} // namespace srsran
