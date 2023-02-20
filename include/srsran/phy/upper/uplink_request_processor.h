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

namespace srsran {

struct prach_buffer_context;
class resource_grid;
struct resource_grid_context;

/// \brief Interface of the uplink request processor.
///
/// The uplink request processor is in charge of setting up the PHY block responsible for the execution of an uplink
/// request (e.g., PRACH detection or PUSCH decoding).
class uplink_request_processor
{
public:
  /// Default destructor.
  virtual ~uplink_request_processor() = default;

  /// \brief Processes the PRACH request using the given configuration and context.
  ///
  /// The PRACH detection results will be notified by the \ref upper_phy_rx_results_notifier::on_new_prach_results
  /// "on_new_prach_results" event notification.
  ///
  /// \param[in] context Context used by the underlying PRACH detector.
  virtual void process_prach_request(const prach_buffer_context& context) = 0;

  /// \brief Processes the uplink request using the given context and grid.
  ///
  /// \param[in] context Resource grid context.
  /// \param[in] grid    Grid in which to store the captured data.
  virtual void process_uplink_slot_request(const resource_grid_context& context, resource_grid& grid) = 0;
};

} // namespace srsran
