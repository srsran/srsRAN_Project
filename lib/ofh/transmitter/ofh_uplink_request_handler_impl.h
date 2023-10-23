/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
#include "ofh_data_flow_cplane_scheduling_commands.h"
#include "srsran/adt/optional.h"
#include "srsran/ofh/transmitter/ofh_uplink_request_handler.h"
#include "srsran/ran/tdd/tdd_ul_dl_config.h"

namespace srsran {
namespace ofh {

/// Uplink request handler implementation configuration.
struct uplink_request_handler_impl_config {
  /// PRACH Contol-Plane enabled flag.
  bool is_prach_cp_enabled;
  /// PRACH eAxC.
  static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> prach_eaxc;
  /// Uplink data eAxC.
  static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> ul_data_eaxc;
  /// Optional TDD configuration.
  optional<tdd_ul_dl_config_common> tdd_config;
  /// Cyclic prefix.
  cyclic_prefix cp;
};

/// Uplink request handler implmentation dependencies.
struct uplink_request_handler_impl_dependencies {
  /// Uplink slot context repository.
  std::shared_ptr<uplink_context_repository> ul_slot_repo;
  /// Uplink PRACH context repository.
  std::shared_ptr<prach_context_repository> ul_prach_repo;
  /// Data flow for Control-Plane scheduling commands.
  std::unique_ptr<data_flow_cplane_scheduling_commands> data_flow;
};

/// Open Fronthaul uplink request handler.
class uplink_request_handler_impl : public uplink_request_handler
{
public:
  explicit uplink_request_handler_impl(const uplink_request_handler_impl_config&  config,
                                       uplink_request_handler_impl_dependencies&& dependencies);

  // See interface for documentation.
  void handle_prach_occasion(const prach_buffer_context& context, prach_buffer& buffer) override;

  // See interface for documentation.
  void handle_new_uplink_slot(const resource_grid_context& context, resource_grid& grid) override;

private:
  bool                                                  is_prach_cp_enabled;
  const cyclic_prefix                                   cp;
  const optional<tdd_ul_dl_config_common>               tdd_config;
  const static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> prach_eaxc;
  const static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> ul_eaxc;
  std::shared_ptr<uplink_context_repository>            ul_slot_repo_ptr;
  std::shared_ptr<prach_context_repository>             ul_prach_repo_ptr;
  uplink_context_repository&                            ul_slot_repo;
  prach_context_repository&                             ul_prach_repo;
  std::unique_ptr<data_flow_cplane_scheduling_commands> data_flow;
};

} // namespace ofh
} // namespace srsran
