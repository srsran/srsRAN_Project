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

#include "dpdk_ethernet_transmitter.h"
#include "srsran/adt/static_vector.h"
#include "srsran/ofh/ethernet/ethernet_gw_config.h"
#include <rte_ethdev.h>

using namespace srsran;
using namespace ether;

void dpdk_transmitter_impl::send(span<span<const uint8_t>> frames)
{
  if (frames.size() >= MAX_BURST_SIZE) {
    logger.warning("Unable to send a transmission burst size of '{}' frames in the DPDK Ethernet transmitter",
                   frames.size());
    return;
  }

  static_vector<::rte_mbuf*, MAX_BURST_SIZE> mbufs(frames.size());
  if (::rte_pktmbuf_alloc_bulk(port_ctx.get_mempool(), mbufs.data(), frames.size()) < 0) {
    logger.warning("Not enough entries in the mempool to send '{}' frames in the DPDK Ethernet transmitter",
                   frames.size());
    return;
  }

  for (unsigned idx = 0, end = frames.size(); idx != end; ++idx) {
    const auto  frame = frames[idx];
    ::rte_mbuf* mbuf  = mbufs[idx];

    if (::rte_pktmbuf_append(mbuf, frame.size()) == nullptr) {
      ::rte_pktmbuf_free(mbuf);
      logger.warning("Unable to append '{}' bytes to the allocated mbuf in the DPDK Ethernet transmitter",
                     frame.size());
      ::rte_pktmbuf_free_bulk(mbufs.data(), mbufs.size());
      return;
    }
    mbuf->data_len = frame.size();
    mbuf->pkt_len  = frame.size();

    uint8_t* data = rte_pktmbuf_mtod(mbuf, uint8_t*);
    std::memcpy(data, frame.data(), frame.size());
  }

  unsigned nof_sent_packets = ::rte_eth_tx_burst(port_ctx.get_port_id(), 0, mbufs.data(), mbufs.size());

  if (SRSRAN_UNLIKELY(nof_sent_packets < mbufs.size())) {
    logger.warning("DPDK dropped '{}' packets out of a total of '{}' in the tx burst",
                   mbufs.size() - nof_sent_packets,
                   mbufs.size());
    for (unsigned buf_idx = nof_sent_packets, last_idx = mbufs.size(); buf_idx != last_idx; ++buf_idx) {
      ::rte_pktmbuf_free(mbufs[buf_idx]);
    }
  }
}
