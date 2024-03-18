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

#include "gtpu_demux_impl.h"
#include "gtpu_pdu.h"
#include <sys/socket.h>

using namespace srsran;

gtpu_demux_impl::gtpu_demux_impl(gtpu_demux_cfg_t cfg_, dlt_pcap& gtpu_pcap_) :
  cfg(cfg_), gtpu_pcap(gtpu_pcap_), logger(srslog::fetch_basic_logger("GTPU"))
{
  logger.info("GTP-U demux. {}", cfg);
}

bool gtpu_demux_impl::add_tunnel(gtpu_teid_t                           teid,
                                 task_executor&                        tunnel_exec,
                                 gtpu_tunnel_rx_upper_layer_interface* tunnel)
{
  std::lock_guard<std::mutex> guard(map_mutex);
  if (teid_to_tunnel.find(teid) != teid_to_tunnel.end()) {
    logger.error("Tunnel already exists. teid={}", teid);
    return false;
  }
  logger.info("Tunnel added. teid={}", teid);
  teid_to_tunnel[teid] = {&tunnel_exec, tunnel};
  return true;
}

bool gtpu_demux_impl::remove_tunnel(gtpu_teid_t teid)
{
  std::lock_guard<std::mutex> guard(map_mutex);
  if (teid_to_tunnel.find(teid) == teid_to_tunnel.end()) {
    logger.error("Tunnel not found. teid={}", teid);
    return false;
  }
  logger.info("Tunnel removed. teid={}", teid);
  teid_to_tunnel.erase(teid);
  return true;
}

void gtpu_demux_impl::handle_pdu(byte_buffer pdu, const sockaddr_storage& src_addr)
{
  uint32_t read_teid = 0;
  if (!gtpu_read_teid(read_teid, pdu, logger)) {
    logger.error("Failed to read TEID from GTP-U PDU. pdu_len={}", pdu.length());
    return;
  }
  gtpu_teid_t teid{read_teid};

  std::lock_guard<std::mutex> guard(map_mutex);
  const auto&                 it = teid_to_tunnel.find(teid);
  if (it == teid_to_tunnel.end()) {
    logger.info("Dropped GTP-U PDU, tunnel not found. teid={}", teid);
    return;
  }

  auto fn = [this, teid, p = std::move(pdu), src_addr]() mutable { handle_pdu_impl(teid, std::move(p), src_addr); };

  if (not it->second.tunnel_exec->defer(std::move(fn))) {
    if (not cfg.warn_on_drop) {
      logger.info("Dropped GTP-U PDU, queue is full. teid={}", teid);
    } else {
      logger.warning("Dropped GTP-U PDU, queue is full. teid={}", teid);
    }
  }
}

void gtpu_demux_impl::handle_pdu_impl(gtpu_teid_t teid, byte_buffer pdu, const sockaddr_storage& src_addr)
{
  if (gtpu_pcap.is_write_enabled()) {
    auto pdu_copy = pdu.deep_copy();
    if (pdu_copy.is_error()) {
      logger.warning("Unable to deep copy PDU for PCAP writer");
    } else {
      gtpu_pcap.push_pdu(std::move(pdu_copy.value()));
    }
  }

  logger.debug(pdu.begin(), pdu.end(), "Forwarding PDU. pdu_len={} teid={}", pdu.length(), teid);

  gtpu_tunnel_rx_upper_layer_interface* tunnel = nullptr;
  {
    /// Get GTP-U tunnel. We lookup the tunnel again, as the tunnel could have been
    /// removed between the time PDU processing was enqueued and the time we actually
    /// run the task.
    std::lock_guard<std::mutex> guard(map_mutex);
    const auto&                 it = teid_to_tunnel.find(teid);
    tunnel                         = it->second.tunnel;
    if (it == teid_to_tunnel.end()) {
      logger.info("Dropped GTP-U PDU, tunnel not found. teid={}", teid);
      return;
    }
  }
  // Forward entire PDU to the tunnel
  // As removal happens in the same thread as handling the PDU, we no longer
  // need the lock.
  tunnel->handle_pdu(std::move(pdu), src_addr);
}
