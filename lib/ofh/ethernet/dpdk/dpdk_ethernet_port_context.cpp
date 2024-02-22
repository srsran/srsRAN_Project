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

#include "dpdk_ethernet_port_context.h"
#include <rte_ethdev.h>

using namespace srsran;
using namespace ether;

/// DPDK configuration settings.
static constexpr unsigned MBUF_CACHE_SIZE = 256;
static constexpr unsigned RX_RING_SIZE    = 1024;
static constexpr unsigned TX_RING_SIZE    = 1024;
static constexpr unsigned NUM_MBUFS       = 8191;

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

/// Configures an Ethernet port managed by DPDK.
static unsigned dpdk_port_configure(const dpdk_port_config& config, ::rte_mempool* mem_pool)
{
  uint16_t port_id;
  if (::rte_eth_dev_get_port_by_name(config.pcie_id.c_str(), &port_id)) {
    ::rte_exit(EXIT_FAILURE,
               "DPDK - Unable to find an Ethernet port with PCIe device id '%s'. Make sure the device id is valid and "
               "is bound to DPDK\n",
               config.pcie_id.c_str());
  }

  if (!port_init(config, mem_pool, port_id)) {
    ::rte_exit(EXIT_FAILURE, "DPDK - Unable to initialize Ethernet port '%u'\n", port_id);
  }

  return port_id;
}

std::shared_ptr<dpdk_port_context> dpdk_port_context::create(const dpdk_port_config& config)
{
  // Create the mbuf pool only once as it is common for all ports.
  static ::rte_mempool* mem_pool = []() {
    ::rte_mempool* pool =
        ::rte_pktmbuf_pool_create("OFH_MBUF_POOL", NUM_MBUFS, MBUF_CACHE_SIZE, 0, MAX_BUFFER_SIZE, ::rte_socket_id());
    if (pool == nullptr) {
      ::rte_exit(EXIT_FAILURE, "DPDK - Unable to create the DPDK mbuf pool\n");
    }
    return pool;
  }();

  return std::shared_ptr<dpdk_port_context>(new dpdk_port_context(dpdk_port_configure(config, mem_pool), mem_pool));
}

dpdk_port_context::~dpdk_port_context()
{
  fmt::print("DPDK - Closing port_id '{}' ...", port_id);
  int ret = ::rte_eth_dev_stop(port_id);
  if (ret != 0) {
    fmt::print("rte_eth_dev_stop: err '{}', port_id '{}'\n", ret, port_id);
  }
  ::rte_eth_dev_close(port_id);
  fmt::print(" Done\n");
}
