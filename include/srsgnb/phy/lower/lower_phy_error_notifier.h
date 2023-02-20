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

#include "srsgnb/phy/support/prach_buffer_context.h"
#include "srsgnb/ran/slot_point.h"

namespace srsgnb {

/// Lower physical layer interface used to notify miscellaneous types of error events.
class lower_phy_error_notifier
{
public:
  /// Default destructor.
  virtual ~lower_phy_error_notifier() = default;

  /// Describes the context in which \c on_late_resource_grid is notified.
  struct late_resource_grid_context {
    /// Sector identifier.
    unsigned sector;
    /// Slot context.
    slot_point slot;
    /// Symbol index within the slot.
    unsigned symbol;
  };

  /// \brief Notifies the unavailability of a downlink resource grid.
  ///
  /// This error happens in a sector when the resource grid for the processing slot is not available at the time when a
  /// symbol is modulated.
  ///
  /// The time window the lower physical layer can receive a resource grid for a slot starts with
  /// lower_phy_timing_notifier::on_tti_boundary() and finishes with the beginning of the processing of the first symbol
  /// within the slot.
  ///
  /// \param[in] context Context in which the resource grid is not available.
  /// \sa lower_phy_rg_handler::handle_resource_grid.
  virtual void on_late_resource_grid(const late_resource_grid_context& context) = 0;

  /// \brief Notifies a PRACH request outside the slot window.
  ///
  /// This error occurs when a PRACH request for slot \f$n\f$ is received after slot \f$n\f$ started being
  /// processed.
  ///
  /// \param[in] context Context of the PRACH request raising the error notification.
  virtual void on_prach_request_late(const prach_buffer_context& context) = 0;

  /// \brief Notifies an excess of PRACH requests.
  ///
  /// This error occurs when the number of pending PRACH requests reaches the limit.
  ///
  /// \param[in] context Context of the PRACH request raising the error notification.
  virtual void on_prach_request_overflow(const prach_buffer_context& context) = 0;
};

} // namespace srsgnb
