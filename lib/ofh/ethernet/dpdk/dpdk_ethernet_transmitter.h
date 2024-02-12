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

#include "dpdk_ethernet_port_context.h"
#include "srsran/ofh/ethernet/ethernet_gateway.h"
#include "srsran/srslog/logger.h"

namespace srsran {
namespace ether {

struct gw_config;

/// DPDK Ethernet transmitter implementation.
class dpdk_transmitter_impl : public gateway
{
public:
  dpdk_transmitter_impl(std::shared_ptr<dpdk_port_context> port_ctx_ptr_, srslog::basic_logger& logger_) :
    logger(logger_), port_ctx_ptr(std::move(port_ctx_ptr_)), port_ctx(*port_ctx_ptr)
  {
    srsran_assert(port_ctx_ptr, "Invalid port context");
  }

  // See interface for documentation.
  void send(span<span<const uint8_t>> frames) override;

private:
  srslog::basic_logger&              logger;
  std::shared_ptr<dpdk_port_context> port_ctx_ptr;
  dpdk_port_context&                 port_ctx;
};

} // namespace ether
} // namespace srsran
