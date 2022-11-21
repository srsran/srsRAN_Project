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

gtpu_demux_impl::gtpu_demux_impl() : logger(srslog::fetch_basic_logger("GTPU")) {}

bool gtpu_demux_impl::add_tunnel(uint32_t teid, gtpu_tunnel_rx_upper_layer_interface* tunnel)
{
  if (teid_to_tunnel.find(teid) != teid_to_tunnel.end()) {
    logger.error("Tunnel with TEID %d already exists", teid);
    return false;
  }
  teid_to_tunnel[teid] = tunnel;
  return true;
}

bool gtpu_demux_impl::remove_tunnel(uint32_t teid)
{
  if (teid_to_tunnel.find(teid) == teid_to_tunnel.end()) {
    logger.error("Tunnel with TEID %d does not exist", teid);
    return false;
  }
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

  const auto& it = teid_to_tunnel.find(teid);
  if (it == teid_to_tunnel.end()) {
    logger.error("Tunnel with TEID %d does not exist. Dropping PDU.", teid);
    return;
  }

  logger.debug("Forwarding GTP-U PDU (len={}B) to teid={}", pdu.length(), teid);

  // Forward entire PDU to the tunnel
  it->second->handle_pdu(std::move(pdu));
}
