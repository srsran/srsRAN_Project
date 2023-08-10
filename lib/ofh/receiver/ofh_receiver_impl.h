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

#include "../support/uplink_context_repository.h"
#include "../support/uplink_cplane_context_repository.h"
#include "ofh_uplane_uplink_packet_handler.h"
#include "ofh_uplane_uplink_symbol_manager.h"
#include "srsran/ofh/ethernet/ethernet_receiver.h"
#include "srsran/ofh/ofh_receiver.h"
#include "srsran/ofh/ofh_receiver_configuration.h"

namespace srsran {
namespace ofh {

/// Open Fronthaul receiver implementation dependencies.
struct receiver_impl_dependencies {
  /// Logger.
  srslog::basic_logger* logger = nullptr;
  /// PRACH context repository.
  std::shared_ptr<uplink_context_repository<ul_prach_context>> prach_context_repo;
  /// UL slot context repository.
  std::shared_ptr<uplink_context_repository<ul_slot_context>> ul_slot_context_repo;
  /// UL Control-Plane context repository.
  std::shared_ptr<uplink_cplane_context_repository> ul_cp_context_repo;
  /// Open Fronthaul User-Plane received symbol notifier.
  uplane_rx_symbol_notifier* notifier = nullptr;
  /// Open Fronthaul IQ data decompressor selector.
  std::unique_ptr<iq_decompressor> decompressor_sel;
  /// Open Fronthaul User-Plane packet decoder.
  std::unique_ptr<uplane_message_decoder> uplane_decoder;
  /// eCPRI packet decoder.
  std::unique_ptr<ecpri::packet_decoder> ecpri_decoder;
  /// Ethernet frame decoder.
  std::unique_ptr<ether::vlan_frame_decoder> eth_frame_decoder;
};

/// \brief Open Fronthaul receiver.
///
/// The Open Fronthaul receiver that groups and owns all the uplink related classes.
class receiver_impl : public receiver
{
public:
  receiver_impl(const receiver_config& config, receiver_impl_dependencies&& depen);

  // See interface for documentation.
  ether::frame_notifier& get_ethernet_frame_notifier() override;

private:
  std::unique_ptr<iq_decompressor> decompressor_sel;
  uplane_uplink_packet_handler     ul_packet_handler;
  uplane_uplink_symbol_manager     ul_symbol_manager;
};

} // namespace ofh
} // namespace srsran
