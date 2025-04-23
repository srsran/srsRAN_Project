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

#include "srsran/ofh/ethernet/dpdk/dpdk_ethernet_port_context.h"
#include <charconv>
#include <optional>
#include <rte_ethdev.h>

using namespace srsran;
using namespace ether;

/// DPDK configuration settings.
static constexpr unsigned MBUF_CACHE_SIZE = 256;
static constexpr unsigned RX_RING_SIZE    = 1024;
static constexpr unsigned TX_RING_SIZE    = 1024;
static constexpr unsigned NUM_MBUFS       = 13824;

/// DPDK port initialization routine.
static bool port_init(const dpdk_port_config& config, ::rte_mempool* mem_pool, unsigned port_id)
{
  uint16_t nb_rxd = RX_RING_SIZE;
  uint16_t nb_txd = TX_RING_SIZE;

  if (::rte_eth_dev_is_valid_port(port_id) == 0) {
    fmt::print("DPDK - Invalid port id '{}'\n", port_id);
    return false;
  }

  ::rte_eth_dev_info dev_info;
  int                ret = ::rte_eth_dev_info_get(port_id, &dev_info);
  if (ret != 0) {
    fmt::print("DPDK - Error getting Ethernet device information: {}\n", port_id, ::strerror(-ret));
    return false;
  }

  ::rte_eth_conf port_conf = {};
  if (dev_info.tx_offload_capa & RTE_ETH_TX_OFFLOAD_MBUF_FAST_FREE) {
    port_conf.txmode.offloads |= RTE_ETH_TX_OFFLOAD_MBUF_FAST_FREE;
  }

  // Configure the Ethernet device.
  if (::rte_eth_dev_configure(port_id, 1, 1, &port_conf) != 0) {
    fmt::print("DPDK - Error configuring Ethernet device\n");
    return false;
  }

  // Configure MTU size.
  if (::rte_eth_dev_set_mtu(port_id, config.mtu_size.value()) != 0) {
    uint16_t current_mtu;
    ::rte_eth_dev_get_mtu(port_id, &current_mtu);
    fmt::print("DPDK - Unable to configure MTU size to '{}' bytes, current MTU size is '{}' bytes\n",
               config.mtu_size,
               current_mtu);
    return false;
  }

  if (::rte_eth_dev_adjust_nb_rx_tx_desc(port_id, &nb_rxd, &nb_txd) != 0) {
    fmt::print("DPDK - Error configuring Ethernet device number of tx/rx descriptors\n");
    return false;
  }

  // Allocate and set up 1 RX queue.
  if (::rte_eth_rx_queue_setup(port_id, 0, nb_rxd, ::rte_eth_dev_socket_id(port_id), nullptr, mem_pool) < 0) {
    fmt::print("DPDK - Error configuring Rx queue\n");
    return false;
  }

  ::rte_eth_txconf txconf = dev_info.default_txconf;
  txconf.offloads         = port_conf.txmode.offloads;
  // Allocate and set up 1 TX queue.
  if (::rte_eth_tx_queue_setup(port_id, 0, nb_txd, ::rte_eth_dev_socket_id(port_id), &txconf) < 0) {
    fmt::print("DPDK - Error configuring Tx queue\n");
    return false;
  }

  // Start Ethernet port.
  if (::rte_eth_dev_start(port_id) < 0) {
    fmt::print("DPDK - Error starting Ethernet device\n");
    return false;
  }

  // Enable RX in promiscuous mode for the Ethernet device.
  if (config.is_promiscuous_mode_enabled) {
    if (::rte_eth_promiscuous_enable(port_id) != 0) {
      fmt::print("DPDK - Error enabling promiscuous mode\n");
      return false;
    }
  }

  return true;
}

/// Checks and prints Ethernet Link status of the given port.
static void print_link_status(unsigned port_id)
{
  ::rte_eth_link link = {};

  if (::rte_eth_link_get(port_id, &link) < 0) {
    fmt::print("DPDK - Failed to retrieve port link status\n");
    return;
  }

  if (link.link_status != RTE_ETH_LINK_UP) {
    fmt::print("DPDK - Port {} link status is \"DOWN\" \n", port_id);
  }
}

static std::optional<int> parse_int(const std::string& value)
{
  int result{};
  auto [ptr, ec] = std::from_chars(value.data(), value.data() + value.size(), result);

  if (ec != std::errc() || ptr != (value.data() + value.size())) {
    return std::nullopt;
  }

  return result;
}

/// On success returns DPDK port identifier resolved based on the passed identifier.
static std::optional<uint16_t> get_dpdk_port_id(const std::string& port_id)
{
  // Try to resolve port identifier based on the passed identifier.
  uint16_t dpdk_port_id;
  if (::rte_eth_dev_get_port_by_name(port_id.c_str(), &dpdk_port_id) == 0) {
    return dpdk_port_id;
  }

  // If the function above failed, try to convert passed parameter to an integer for the case when DPDK port identifier
  // was specified directly in the config.
  if (auto result = parse_int(port_id); result && *result >= 0) {
    return result;
  }

  return std::nullopt;
}

/// Configures an Ethernet port managed by DPDK.
static unsigned dpdk_port_configure(const dpdk_port_config& config, ::rte_mempool* mem_pool)
{
  auto opt_port_id = get_dpdk_port_id(config.id);
  if (!opt_port_id) {
    ::rte_exit(EXIT_FAILURE,
               "DPDK - Unable to find an Ethernet port with device id '%s'. Make sure the device id is valid and "
               "is bound to DPDK\n",
               config.id.c_str());
  }

  uint16_t dpdk_port_id = *opt_port_id;
  if (!port_init(config, mem_pool, dpdk_port_id)) {
    ::rte_exit(EXIT_FAILURE, "DPDK - Unable to initialize Ethernet port '%u'\n", dpdk_port_id);
  }

  if (config.is_link_status_check_enabled) {
    print_link_status(dpdk_port_id);
  }
  return dpdk_port_id;
}

std::shared_ptr<dpdk_port_context> dpdk_port_context::create(const dpdk_port_config& config)
{
  // Create the mbuf pool only once as it is common for all ports.
  static ::rte_mempool* mem_pool = [&config]() {
    ::rte_mempool* pool = ::rte_pktmbuf_pool_create(
        "OFH_MBUF_POOL", NUM_MBUFS, MBUF_CACHE_SIZE, 0, (MAX_BUFFER_SIZE + RTE_PKTMBUF_HEADROOM), ::rte_socket_id());
    if (pool == nullptr) {
      ::rte_exit(EXIT_FAILURE, "DPDK - Unable to create the DPDK mbuf pool for port '%s'\n", config.id.c_str());
    }
    return pool;
  }();

  return std::shared_ptr<dpdk_port_context>(
      new dpdk_port_context(config.id, dpdk_port_configure(config, mem_pool), mem_pool));
}

dpdk_port_context::~dpdk_port_context()
{
  fmt::print("DPDK - Closing port '{}', id = '{}' ... ", port_id, dpdk_port_id);
  int ret = ::rte_eth_dev_stop(dpdk_port_id);
  if (ret != 0) {
    fmt::print("rte_eth_dev_stop: err '{}', port_id '{}'\n", ret, dpdk_port_id);
  }
  ret = ::rte_eth_dev_close(dpdk_port_id);
  if (ret != 0) {
    fmt::print("rte_eth_dev_close: err '{}', port_id '{}'\n", rte_errno, dpdk_port_id);
  }
  ::rte_mempool_free(mem_pool);

  fmt::print(" Done\n");
}
