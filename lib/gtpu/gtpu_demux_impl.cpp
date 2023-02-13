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
#include "srsgnb/gtpu/gtpu_demux.h"

using namespace srsgnb;

gtpu_demux_impl::gtpu_demux_impl(task_executor& cu_up_exec_) :
  cu_up_exec(cu_up_exec_), logger(srslog::fetch_basic_logger("GTPU"))
{
}

bool gtpu_demux_impl::add_tunnel(uint32_t teid, gtpu_tunnel_rx_upper_layer_interface* tunnel)
{
  if (teid_to_tunnel.find(teid) != teid_to_tunnel.end()) {
    logger.error("Tunnel with TEID={:#x} already exists", teid);
    return false;
  }
  logger.info("Adding tunnel with TEID={:#x}", teid);
  teid_to_tunnel[teid] = tunnel;
  return true;
}

bool gtpu_demux_impl::remove_tunnel(uint32_t teid)
{
  if (teid_to_tunnel.find(teid) == teid_to_tunnel.end()) {
    logger.error("Tunnel with TEID={:#x} does not exist", teid);
    return false;
  }
  logger.info("Removing tunnel with TEID={:#x}", teid);
  teid_to_tunnel.erase(teid);
  return true;
}

void gtpu_demux_impl::handle_pdu(byte_buffer pdu)
{
  uint32_t teid = 0;
  if (gtpu_read_teid(teid, pdu, logger) == false) {
    logger.error("Failed to read TEID from GTP-U PDU");
    return;
  }

  auto fn = [this, teid, p = std::move(pdu)]() mutable { handle_pdu_impl(teid, std::move(p)); };
  if (not cu_up_exec.execute(std::move(fn))) {
    logger.info("TEID={:#x}: Dropping GTP-U PDU. Cause: GTP-U DEMUX PDU queue is full.", teid);
  }
}

void gtpu_demux_impl::handle_pdu_impl(uint32_t teid, byte_buffer pdu)
{
  const auto& it = teid_to_tunnel.find(teid);
  if (it == teid_to_tunnel.end()) {
    logger.error("Tunnel with TEID={:#x} does not exist. Dropping PDU.", teid);
    return;
  }
  logger.debug(pdu.begin(), pdu.end(), "Forwarding PDU (len={}B) to TEID={:#x}", pdu.length(), teid);

  // Forward entire PDU to the tunnel
  it->second->handle_pdu(std::move(pdu));
}
