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

#include "srsgnb/ran/slot_point.h"

namespace srsran {

class prach_buffer;
struct prach_buffer_context;
class resource_grid;
struct resource_grid_context;
class resource_grid_reader;

/// RU uplink received symbol context.
struct ru_uplink_rx_symbol_context {
  /// Slot context.
  slot_point slot;
  /// Radio sector identifier.
  unsigned sector;
  /// Index, within the slot, of the last processed symbol.
  unsigned symbol_id;
};

/// Radio Unit uplink plane symbol reception notifier.
class ru_uplink_plane_rx_symbol_notifier
{
public:
  virtual ~ru_uplink_plane_rx_symbol_notifier() = default;

  /// \brief Notifies the completion of an OFDM symbol for a given context.
  ///
  /// \param[in] context Notification context.
  /// \param[in] grid    Resource grid that belongs to the context.
  virtual void on_new_uplink_symbol(const ru_uplink_rx_symbol_context& context, const resource_grid_reader& grid) = 0;

  /// \brief Notifies the completion of a PRACH window.
  ///
  /// The RU uses this method to notify that the PRACH window identified by \c context has been written in \c buffer.
  ///
  /// \param[in] context PRACH context.
  /// \param[in] buffer  Read-only PRACH buffer.
  virtual void on_new_prach_window_data(const prach_buffer_context& context, const prach_buffer& buffer) = 0;
};

/// \brief Radio Unit uplink plane handler.
///
/// Handles PRACH and uplink data requests and captures uplink data. The uplink received data will be notified through
/// the \ref ru_uplink_plane_rx_symbol_notifier notifier.
class ru_uplink_plane_handler
{
public:
  virtual ~ru_uplink_plane_handler() = default;

  /// \brief Requests the RU to capture a PRACH window.
  ///
  /// The RU must capture the PHY window identified by \c context.
  ///
  /// \param[in] context PRACH window context.
  /// \param[in] buffer  PRACH buffer used to write the PRACH window.
  virtual void handle_prach_occasion(const prach_buffer_context& context, prach_buffer& buffer) = 0;

  /// \brief Requests the RU to provide an uplink slot.
  ///
  /// The RU must process the slot described by \c context.
  ///
  /// \param[in] context Resource grid context.
  /// \param[in] buffer  Resource grid to store the processed slot.
  virtual void handle_new_uplink_slot(const resource_grid_context& context, resource_grid& grid) = 0;
};

} // namespace srsran