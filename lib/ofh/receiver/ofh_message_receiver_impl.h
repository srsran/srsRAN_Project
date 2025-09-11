/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "../operation_controller_dummy.h"
#include "ofh_closed_rx_window_handler.h"
#include "ofh_data_flow_uplane_uplink_data.h"
#include "ofh_data_flow_uplane_uplink_prach.h"
#include "ofh_message_receiver.h"
#include "ofh_message_receiver_metrics_collector.h"
#include "ofh_sequence_id_checker_impl.h"
#include "srsran/adt/static_vector.h"
#include "srsran/ofh/ecpri/ecpri_packet_decoder.h"
#include "srsran/ofh/ethernet/ethernet_receiver.h"
#include "srsran/ofh/ethernet/vlan_ethernet_frame_decoder.h"
#include "srsran/ofh/ofh_constants.h"
#include "srsran/ofh/ofh_controller.h"
#include "srsran/ofh/serdes/ofh_message_properties.h"
#include "srsran/ofh/serdes/ofh_uplane_message_decoder.h"
#include "srsran/srslog/logger.h"

namespace srsran {
namespace ofh {

class rx_window_checker;

/// Message receiver configuration.
struct message_receiver_config {
  /// Radio sector identifier.
  unsigned sector;
  /// Number of symbols
  unsigned nof_symbols;
  /// Subcarrier spacing.
  subcarrier_spacing scs;
  /// VLAN ethernet frame parameters.
  ether::vlan_frame_params vlan_params;
  /// Uplink PRACH eAxC.
  static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> prach_eaxc;
  /// Uplink eAxC.
  static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> ul_eaxc;
  /// Warn unreceived Open Fronthaul messages.
  warn_unreceived_ru_frames warn_unreceived_frames = warn_unreceived_ru_frames::after_traffic_detection;
  /// If set to true, metrics are enabled in the message receiver.
  bool are_metrics_enabled = false;
  /// If set to true, logs late events as warnings, otherwise as info.
  bool enable_log_warnings_for_lates;
};

/// Message receiver dependencies.
struct message_receiver_dependencies {
  /// Logger.
  srslog::basic_logger* logger = nullptr;
  /// Ethernet receiver.
  std::unique_ptr<ether::receiver> eth_receiver;
  /// Reception window checker.
  rx_window_checker* window_checker = nullptr;
  /// Reception window handler.
  closed_rx_window_handler* window_handler;
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

/// Open Fronthaul message receiver interface implementation.
class message_receiver_impl : public message_receiver
{
public:
  message_receiver_impl(const message_receiver_config& config, message_receiver_dependencies&& dependencies);

  // See interface for documentation.
  void on_new_frame(ether::unique_rx_buffer buffer) override;

  // See interface for the documentation.
  operation_controller& get_operation_controller() override { return controller; }

  // See interface for the documentation.
  message_receiver_metrics_collector* get_metrics_collector() override
  {
    return metrics_collector.enabled() ? &metrics_collector : nullptr;
  }

private:
  /// Processes an Ethernet frame received from the underlying Ethernet link.
  void process_new_frame(ether::unique_rx_buffer buff);

  /// Returns true if the ethernet frame represented by the given eth parameters should be filtered, otherwise false.
  bool should_ethernet_frame_be_filtered(const ether::vlan_frame_params& eth_params) const;

  /// Returns true if the eCPRI packet represented by the given eCPRI parameters should be filtered, otherwise false.
  bool should_ecpri_packet_be_filtered(const ecpri::packet_parameters& ecpri_params) const;

private:
  srslog::basic_logger&                                 logger;
  const unsigned                                        sector_id;
  const unsigned                                        nof_symbols;
  const subcarrier_spacing                              scs;
  const ether::vlan_frame_params                        vlan_params;
  const static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> ul_prach_eaxc;
  const static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> ul_eaxc;
  bool                                                  warn_unreceived_frames_on_first_rx_message;
  rx_window_checker&                                    window_checker;
  closed_rx_window_handler&                             window_handler;
  std::unique_ptr<sequence_id_checker>                  seq_id_checker;
  std::unique_ptr<ether::vlan_frame_decoder>            vlan_decoder;
  std::unique_ptr<ecpri::packet_decoder>                ecpri_decoder;
  std::unique_ptr<data_flow_uplane_uplink_data>         data_flow_uplink;
  std::unique_ptr<data_flow_uplane_uplink_prach>        data_flow_prach;
  message_receiver_metrics_collector                    metrics_collector;
  bool                                                  enable_log_warnings_for_lates;
  operation_controller_dummy                            controller;
};

} // namespace ofh
} // namespace srsran
