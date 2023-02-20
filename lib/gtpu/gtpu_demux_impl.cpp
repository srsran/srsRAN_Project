/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "gtpu_demux_impl.h"
#include "gtpu_pdu.h"

using namespace srsran;

gtpu_demux_impl::gtpu_demux_impl(task_executor& cu_up_exec_) :
  cu_up_exec(cu_up_exec_), logger(srslog::fetch_basic_logger("GTPU"))
{
}

bool gtpu_demux_impl::add_tunnel(uint32_t teid, gtpu_tunnel_rx_upper_layer_interface* tunnel)
{
  if (teid_to_tunnel.find(teid) != teid_to_tunnel.end()) {
    logger.error("Tunnel already exists. teid={:#x}", teid);
    return false;
  }
  logger.info("Tunnel added. teid={:#x}", teid);
  teid_to_tunnel[teid] = tunnel;
  return true;
}

bool gtpu_demux_impl::remove_tunnel(uint32_t teid)
{
  if (teid_to_tunnel.find(teid) == teid_to_tunnel.end()) {
    logger.error("Tunnel not found. teid={:#x}", teid);
    return false;
  }
  logger.info("Tunnel removed. teid={:#x}", teid);
  teid_to_tunnel.erase(teid);
  return true;
}

void gtpu_demux_impl::handle_pdu(byte_buffer pdu)
{
  uint32_t teid = 0;
  if (gtpu_read_teid(teid, pdu, logger) == false) {
    logger.error("Failed to read TEID from GTP-U PDU. pdu_len={}", pdu.length());
    return;
  }

  auto fn = [this, teid, p = std::move(pdu)]() mutable { handle_pdu_impl(teid, std::move(p)); };
  if (not cu_up_exec.execute(std::move(fn))) {
    logger.info("Dropped GTP-U PDU, queue is full. teid={:#x}", teid);
  }
}

void gtpu_demux_impl::handle_pdu_impl(uint32_t teid, byte_buffer pdu)
{
  const auto& it = teid_to_tunnel.find(teid);
  if (it == teid_to_tunnel.end()) {
    logger.error("Dropped GTP-U PDU, tunnel not found. teid={:#x}", teid);
    return;
  }
  logger.debug(pdu.begin(), pdu.end(), "Forwarding PDU. pdu_len={} teid={:#x}", pdu.length(), teid);

  // Forward entire PDU to the tunnel
  it->second->handle_pdu(std::move(pdu));
}
