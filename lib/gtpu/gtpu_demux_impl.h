/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/gtpu/gtpu_demux.h"
#include "srsgnb/support/executors/task_executor.h"
#include <memory>
#include <unordered_map>

namespace srsran {

class gtpu_demux_impl final : public gtpu_demux
{
public:
  explicit gtpu_demux_impl(task_executor& cu_up_exec);
  ~gtpu_demux_impl() = default;

  // gtpu_demux_rx_upper_layer_interface
  void handle_pdu(byte_buffer pdu) override; // Will be run from IO executor.

  // gtpu_demux_ctrl
  bool add_tunnel(uint32_t teid, gtpu_tunnel_rx_upper_layer_interface* tunnel) override;
  bool remove_tunnel(uint32_t teid) override;

private:
  // Actual demuxing, to be run in CU-UP executor.
  void handle_pdu_impl(uint32_t teid, byte_buffer pdu);

  task_executor& cu_up_exec;

  ///< TODO: revisit mapping of TEID to executors, one executor per UE should be doable.
  std::unordered_map<uint16_t, gtpu_tunnel_rx_upper_layer_interface*> teid_to_tunnel; ///< Map TEID on GTP-U entity.

  srslog::basic_logger& logger;
};

} // namespace srsran
