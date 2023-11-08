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

#include "../support/prach_context_repository.h"
#include "../support/uplink_context_repository.h"
#include "../support/uplink_cplane_context_repository.h"
#include "ofh_message_receiver.h"
#include "ofh_receiver_controller.h"
#include "ofh_rx_window_checker.h"
#include "srsran/ofh/receiver/ofh_receiver.h"
#include "srsran/ofh/receiver/ofh_receiver_configuration.h"

namespace srsran {
namespace ofh {

/// Open Fronthaul receiver implementation dependencies.
struct receiver_impl_dependencies {
  /// Logger.
  srslog::basic_logger* logger = nullptr;
  /// eCPRI packet decoder.
  std::unique_ptr<ecpri::packet_decoder> ecpri_decoder;
  /// Ethernet frame decoder.
  std::unique_ptr<ether::vlan_frame_decoder> eth_frame_decoder;
  /// Open Fronthaul User-Plane decoder.
  std::unique_ptr<uplane_message_decoder> uplane_decoder;
  /// User-Plane uplink data flow.
  std::unique_ptr<data_flow_uplane_uplink_data> data_flow_uplink;
  /// User-Plane uplink PRACH data flow.
  std::unique_ptr<data_flow_uplane_uplink_prach> data_flow_prach;
};

/// \brief Open Fronthaul receiver.
///
/// The Open Fronthaul receiver that groups and owns all the uplink related classes.
class receiver_impl : public receiver
{
public:
  receiver_impl(const receiver_config& config, receiver_impl_dependencies&& dependencies);

  // See interface for documentation.
  ether::frame_notifier& get_ethernet_frame_notifier() override;

  // See interface for documentation.
  ota_symbol_handler& get_ota_symbol_handler() override;

  // See interface for documentation.
  controller& get_controller() override;

  /// Sets the Ethernet receiver for this Open Fronthaul receiver.
  void set_ethernet_receiver(std::unique_ptr<ether::receiver> eth_rx)
  {
    msg_receiver.set_ethernet_receiver(std::move(eth_rx));
  }

private:
  rx_window_checker   window_checker;
  message_receiver    msg_receiver;
  receiver_controller ctrl;
};

} // namespace ofh
} // namespace srsran
