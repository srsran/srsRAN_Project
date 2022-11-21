/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/gtpu/gtpu_demux.h"
#include <memory>
#include <unordered_map>

namespace srsgnb {

class gtpu_demux_impl final : public gtpu_demux
{
public:
  gtpu_demux_impl();
  ~gtpu_demux_impl() = default;

  // gtpu_demux_rx_upper_layer_interface
  void handle_pdu(byte_buffer pdu) override;

  // gtpu_demux_ctrl
  bool add_tunnel(uint32_t teid, gtpu_tunnel_rx_upper_layer_interface* tunnel) override;
  bool remove_tunnel(uint32_t teid) override;

private:
  srslog::basic_logger& logger;

  ///< TODO: revisit mutexing for protecting the tunnels map (this should be a shared mutex)
  std::unordered_map<uint16_t, gtpu_tunnel_rx_upper_layer_interface*> teid_to_tunnel; ///< Map TEID on GTP-U entity.
};

} // namespace srsgnb
