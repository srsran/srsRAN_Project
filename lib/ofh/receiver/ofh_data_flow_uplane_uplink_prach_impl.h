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

#include "../support/prach_context_repository.h"
#include "../support/uplink_cplane_context_repository.h"
#include "ofh_data_flow_uplane_decoding_metrics_collector.h"
#include "ofh_data_flow_uplane_uplink_prach.h"
#include "ofh_uplane_prach_data_flow_notifier.h"
#include "ofh_uplane_prach_symbol_data_flow_writer.h"
#include "srsran/ofh/ofh_constants.h"
#include "srsran/ofh/serdes/ofh_uplane_message_decoder.h"

namespace srsran {
namespace ofh {

/// Open Fronthaul User-Plane uplink PRACH data flow implementation configuration.
struct data_flow_uplane_uplink_prach_impl_config {
  /// Radio sector identifier.
  unsigned sector;
  /// PRACH Control-Plane enabled flag.
  bool is_prach_cplane_enabled;
  /// Ignore the start symbol value received in the PRACH U-Plane packets.
  bool ignore_prach_start_symbol;
  /// Enabled metrics flag.
  bool are_metrics_enabled;
  /// Uplink PRACH eAxCs.
  static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> prach_eaxcs;
};

/// Open Fronthaul User-Plane uplink PRACH data flow implementation dependencies.
struct data_flow_uplane_uplink_prach_impl_dependencies {
  /// Logger.
  srslog::basic_logger* logger;
  /// User-Plane received symbol notifier.
  std::shared_ptr<uplane_rx_symbol_notifier> notifier;
  /// Control-Plane context repository.
  std::shared_ptr<uplink_cplane_context_repository> prach_cplane_context_repo;
  /// Uplink PRACH context repository.
  std::shared_ptr<prach_context_repository> prach_context_repo;
  /// User-Plane message decoder.
  std::unique_ptr<uplane_message_decoder> uplane_decoder;
};

/// Open Fronthaul User-Plane uplink PRACH data flow implementation.
class data_flow_uplane_uplink_prach_impl : public data_flow_uplane_uplink_prach
{
public:
  data_flow_uplane_uplink_prach_impl(const data_flow_uplane_uplink_prach_impl_config&  config,
                                     data_flow_uplane_uplink_prach_impl_dependencies&& dependencies);

  // See interface for documentation.
  void decode_type1_message(unsigned eaxc, span<const uint8_t> message) override;

  // See interface for documentation.
  data_flow_message_decoding_metrics_collector& get_metrics_collector() override { return metrics_collector; }

private:
  /// Returns true if the User-Plane packet represented by the given User-Plane results and eAxC should be filtered,
  /// otherwise false.
  bool should_uplane_packet_be_filtered(unsigned eaxc, const uplane_message_decoder_results& results) const;

private:
  srslog::basic_logger&                             logger;
  const bool                                        is_prach_cplane_enabled;
  std::shared_ptr<uplink_cplane_context_repository> prach_cplane_context_repo;
  std::unique_ptr<uplane_message_decoder>           uplane_decoder;
  uplane_prach_symbol_data_flow_writer              prach_iq_writter;
  uplane_prach_data_flow_notifier                   notification_sender;
  const unsigned                                    sector_id;
  const bool                                        ignore_prach_start_symbol;
  data_flow_message_decoding_metrics_collector      metrics_collector;
};

} // namespace ofh
} // namespace srsran
