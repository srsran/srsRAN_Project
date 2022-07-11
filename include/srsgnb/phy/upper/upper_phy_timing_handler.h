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

class slot_point;

/// Describes the context of the current timing boundary.
struct upper_phy_timing_context {
  /// Indicates the current slot.
  slot_point slot;
};

/// Upper physical layer's handle to align timing boundaries.
class upper_phy_timing_handler
{
public:
  /// Default destructor.
  virtual ~upper_phy_timing_handler() = default;

  /// \brief Handles a new TTI boundary event.
  /// \param [in] context Provides the notification context.
  virtual void handle_tti_boundary(const upper_phy_timing_context& context) = 0;

  /// \brief Handles a half slot (sym 7) packet arrival.
  /// \param [in] context Provides the notification context.
  virtual void handle_ul_half_slot_boundary(const upper_phy_timing_context& context) = 0;

  /// \brief Handles a full slot (sym 14) packet arrival.
  /// \param [in] context Provides the notification context.
  virtual void handle_ul_full_slot_boundary(const upper_phy_timing_context& context) = 0;
};

} // namespace srsgnb
