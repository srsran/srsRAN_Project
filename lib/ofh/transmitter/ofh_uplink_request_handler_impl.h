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
#include "ofh_data_flow_cplane_scheduling_commands.h"
#include "srsran/adt/optional.h"
#include "srsran/ofh/transmitter/ofh_uplink_request_handler.h"

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
  /// Uplink slot context repository.
  std::shared_ptr<uplink_context_repository<ul_slot_context>> ul_slot_repo;
  /// Uplink PRACH context repository.
  std::shared_ptr<uplink_context_repository<ul_prach_context>> ul_prach_repo;
  /// Data flow for Control-Plane scheduling commands.
  std::unique_ptr<data_flow_cplane_scheduling_commands> data_flow;
};

/// Open Fronthaul uplink request handler.
class uplink_request_handler_impl : public uplink_request_handler
{
public:
  explicit uplink_request_handler_impl(uplink_request_handler_impl_config&& config);

  // See interface for documentation.
  void handle_prach_occasion(const prach_buffer_context& context, prach_buffer& buffer) override;

  // See interface for documentation.
  void handle_new_uplink_slot(const resource_grid_context& context, resource_grid& grid) override;

private:
  bool                                                         is_prach_cp_enabled;
  const static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC>        prach_eaxc;
  const static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC>        ul_eaxc;
  std::shared_ptr<uplink_context_repository<ul_slot_context>>  ul_slot_repo_ptr;
  std::shared_ptr<uplink_context_repository<ul_prach_context>> ul_prach_repo_ptr;
  uplink_context_repository<ul_slot_context>&                  ul_slot_repo;
  uplink_context_repository<ul_prach_context>&                 ul_prach_repo;
  std::unique_ptr<data_flow_cplane_scheduling_commands>        data_flow;
};

} // namespace ofh
} // namespace srsran
