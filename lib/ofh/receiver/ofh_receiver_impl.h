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
  /// Ethernet receiver.
  std::unique_ptr<ether::receiver> eth_receiver;
  /// eCPRI packet decoder.
  std::unique_ptr<ecpri::packet_decoder> ecpri_decoder;
  /// Ethernet frame decoder.
  std::unique_ptr<ether::vlan_frame_decoder> eth_frame_decoder;
  /// User-Plane uplink data flow.
  std::unique_ptr<data_flow_uplane_uplink_data> data_flow_uplink;
  /// User-Plane uplink PRACH data flow.
  std::unique_ptr<data_flow_uplane_uplink_prach> data_flow_prach;
  /// Sequence id checker.
  std::unique_ptr<sequence_id_checker> seq_id_checker;
};

/// \brief Open Fronthaul receiver.
///
/// The Open Fronthaul receiver that groups and owns all the uplink related classes.
class receiver_impl : public receiver
{
public:
  receiver_impl(const receiver_config& config, receiver_impl_dependencies&& dependencies);

  // See interface for documentation.
  ota_symbol_boundary_notifier* get_ota_symbol_boundary_notifier() override;

  // See interface for documentation.
  controller& get_controller() override;

private:
  rx_window_checker   window_checker;
  message_receiver    msg_receiver;
  receiver_controller ctrl;
};

} // namespace ofh
} // namespace srsran
