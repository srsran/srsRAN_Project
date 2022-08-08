/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

namespace srsgnb {

struct prach_buffer_context;

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
};

} // namespace srsgnb
