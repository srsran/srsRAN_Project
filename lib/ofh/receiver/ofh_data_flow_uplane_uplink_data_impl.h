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

#include "../support/uplink_context_repository.h"
#include "../support/uplink_cplane_context_repository.h"
#include "ofh_data_flow_uplane_uplink_data.h"
#include "ofh_uplane_rx_symbol_data_flow_notifier.h"
#include "ofh_uplane_rx_symbol_data_flow_writer.h"
#include "srsran/adt/static_vector.h"
#include "srsran/ofh/ofh_constants.h"
#include "srsran/ofh/serdes/ofh_uplane_message_decoder.h"

namespace srsran {
namespace ofh {

/// Open Fronthaul User-Plane uplink data flow implementation configuration.
struct data_flow_uplane_uplink_data_impl_config {
  /// Uplink eAxCs.
  static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> ul_eaxc;
};

/// Open Fronthaul User-Plane uplink data flow implementation dependencies.
struct data_flow_uplane_uplink_data_impl_dependencies {
  /// Logger.
  srslog::basic_logger* logger;
  /// User-Plane received symbol notifier.
  std::shared_ptr<uplane_rx_symbol_notifier> notifier;
  /// Control-Plane context repository.
  std::shared_ptr<uplink_cplane_context_repository> ul_cplane_context_repo_ptr;
  /// Uplink context repository.
  std::shared_ptr<uplink_context_repository> ul_context_repo;
  /// User-Plane message decoder.
  std::unique_ptr<uplane_message_decoder> uplane_decoder;
};

/// Open Fronthaul User-Plane uplink data flow implementation.
class data_flow_uplane_uplink_data_impl : public data_flow_uplane_uplink_data
{
public:
  data_flow_uplane_uplink_data_impl(const data_flow_uplane_uplink_data_impl_config&  config,
                                    data_flow_uplane_uplink_data_impl_dependencies&& dependencies);

  // See interface for documentation.
  void decode_type1_message(unsigned eaxc, span<const uint8_t> message) override;

private:
  /// Returns true if the User-Plane packet represented by the given User-Plane results and eAxC should be filtered,
  /// otherwise false.
  bool should_uplane_packet_be_filtered(unsigned eaxc, const uplane_message_decoder_results& results) const;

private:
  srslog::basic_logger&                             logger;
  std::shared_ptr<uplink_cplane_context_repository> ul_cplane_context_repo_ptr;
  uplink_cplane_context_repository&                 ul_cplane_context_repo;
  std::unique_ptr<uplane_message_decoder>           uplane_decoder;
  uplane_rx_symbol_data_flow_writer                 rx_symbol_writer;
  uplane_rx_symbol_data_flow_notifier               notification_sender;
};

} // namespace ofh
} // namespace srsran
