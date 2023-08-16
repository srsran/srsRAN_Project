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

#include "srsran/gtpu/gtpu_demux.h"
#include "srsran/pcap/pcap.h"
#include "srsran/support/executors/task_executor.h"
#include <memory>
#include <unordered_map>

namespace srsran {

class gtpu_demux_impl final : public gtpu_demux
{
public:
  explicit gtpu_demux_impl(task_executor& cu_up_exec, dlt_pcap& gtpu_pcap_);
  ~gtpu_demux_impl() = default;

  // gtpu_demux_rx_upper_layer_interface
  void handle_pdu(byte_buffer pdu, sockaddr_storage& src_addr) override; // Will be run from IO executor.

  // gtpu_demux_ctrl
  bool add_tunnel(gtpu_teid_t teid, gtpu_tunnel_rx_upper_layer_interface* tunnel) override;
  bool remove_tunnel(gtpu_teid_t teid) override;

private:
  // Actual demuxing, to be run in CU-UP executor.
  void handle_pdu_impl(gtpu_teid_t teid, byte_buffer pdu, sockaddr_storage src_addr);

  task_executor& cu_up_exec;
  dlt_pcap&      gtpu_pcap;

  ///< TODO: revisit mapping of TEID to executors, one executor per UE should be doable.
  std::unordered_map<gtpu_teid_t, gtpu_tunnel_rx_upper_layer_interface*, gtpu_teid_hasher_t> teid_to_tunnel;

  srslog::basic_logger& logger;
};

} // namespace srsran
