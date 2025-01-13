/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/support/units.h"
#include <memory>

struct rte_mempool;

namespace srsran {
namespace ether {

/// DPDK configuration settings.
constexpr unsigned MAX_BURST_SIZE  = 64;
constexpr unsigned MAX_BUFFER_SIZE = 9600;

/// DPDK port configuration.
struct dpdk_port_config {
  /// Device identifier.
  std::string id;
  /// Promiscuous mode flag.
  bool is_promiscuous_mode_enabled;
  /// Ethernet link status checking flag.
  bool is_link_status_check_enabled;
  /// MTU size.
  units::bytes mtu_size;
};

/// \brief DPDK Ethernet port context.
///
/// Encapsulates and manages the lifetime of the internal DPDK resources of an Ethernet port.
class dpdk_port_context
{
  dpdk_port_context(const std::string& port_id_, unsigned dpdk_port_id_, ::rte_mempool* mem_pool_) :
    port_id(port_id_), dpdk_port_id(dpdk_port_id_), mem_pool(mem_pool_)
  {
  }

public:
  /// Creates and initializes a new DPDK port context with the given configuration.
  static std::shared_ptr<dpdk_port_context> create(const dpdk_port_config& config);

  ~dpdk_port_context();

  /// Returns the DPDK port identifier of this context .
  unsigned get_dpdk_port_id() const { return dpdk_port_id; }

  /// Returns the string port identifier of this context.
  const std::string& get_port_id() const { return port_id; }

  /// Returns the mbuf memory pool of this context.
  ::rte_mempool* get_mempool() { return mem_pool; }

  /// Returns the mbuf memory pool of this context.
  const ::rte_mempool* get_mempool() const { return mem_pool; }

private:
  const std::string    port_id;
  const unsigned       dpdk_port_id;
  ::rte_mempool* const mem_pool;
};

} // namespace ether
} // namespace srsran
