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

#include "ofh_data_flow_uplane_uplink_data.h"
#include "ofh_data_flow_uplane_uplink_prach.h"
#include "ofh_sequence_id_checker_impl.h"
#include "srsran/adt/static_vector.h"
#include "srsran/ofh/ecpri/ecpri_packet_decoder.h"
#include "srsran/ofh/ethernet/ethernet_frame_notifier.h"
#include "srsran/ofh/ethernet/ethernet_receiver.h"
#include "srsran/ofh/ethernet/vlan_ethernet_frame_decoder.h"
#include "srsran/ofh/ofh_constants.h"
#include "srsran/ofh/serdes/ofh_message_properties.h"
#include "srsran/ofh/serdes/ofh_uplane_message_decoder.h"
#include "srsran/srslog/logger.h"

namespace srsran {
namespace ofh {

class rx_window_checker;

/// Message receiver configuration.
struct message_receiver_config {
  /// VLAN ethernet frame parameters.
  ether::vlan_frame_params vlan_params;
  /// Uplink PRACH eAxC.
  static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> prach_eaxc;
  /// Uplink eAxC.
  static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> ul_eaxc;
};

/// Message receiver dependencies.
struct message_receiver_dependencies {
  /// Logger.
  srslog::basic_logger* logger;
  /// Reception window checker.
  rx_window_checker* window_checker;
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
  /// Sequence id checker.
  std::unique_ptr<sequence_id_checker> seq_id_checker;
};

/// Open Fronthaul message receiver.
///
/// This class listens to incoming Ethernet frames and decodes them as Open Fronthaul messages. Once a new message is
/// detected, is it handled to the corresponding data flow for further processing.
class message_receiver : public ether::frame_notifier
{
public:
  message_receiver(const message_receiver_config& config, message_receiver_dependencies&& dependencies);

  // See interface for documentation.
  void on_new_frame(span<const uint8_t> payload) override;

  /// Sets the Ethernet receiver for this Open Fronthaul message receiver.
  void set_ethernet_receiver(std::unique_ptr<ether::receiver> eth_rx)
  {
    srsran_assert(!eth_receiver, "Ethernet receiver already set");
    srsran_assert(eth_rx, "Invalid Ethernet receiver to set");
    eth_receiver = std::move(eth_rx);
  }

  /// Returns the Ethernet receiver of this Open Fronthaul message receiver.
  ether::receiver& get_ethernet_receiver()
  {
    srsran_assert(eth_receiver, "Ethernet receiver is not configured");
    return *eth_receiver;
  }

private:
  /// Returns true if the ethernet frame represented by the given eth parameters should be filtered, otherwise false.
  bool should_ethernet_frame_be_filtered(const ether::vlan_frame_params& eth_params) const;

  /// Returns true if the eCPRI packet represented by the given eCPRI parameters should be filtered, otherwise false.
  bool should_ecpri_packet_be_filtered(const ecpri::packet_parameters& ecpri_params) const;

private:
  srslog::basic_logger&                                 logger;
  const ether::vlan_frame_params                        vlan_params;
  const static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> ul_prach_eaxc;
  const static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> ul_eaxc;
  rx_window_checker&                                    window_checker;
  std::unique_ptr<sequence_id_checker>                  seq_id_checker;
  std::unique_ptr<ether::vlan_frame_decoder>            vlan_decoder;
  std::unique_ptr<ecpri::packet_decoder>                ecpri_decoder;
  std::unique_ptr<uplane_message_decoder>               uplane_decoder;
  std::unique_ptr<data_flow_uplane_uplink_data>         data_flow_uplink;
  std::unique_ptr<data_flow_uplane_uplink_prach>        data_flow_prach;
  std::unique_ptr<ether::receiver>                      eth_receiver;
};

} // namespace ofh
} // namespace srsran
