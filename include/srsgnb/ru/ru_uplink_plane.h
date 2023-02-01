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

#include "srsgnb/ran/slot_point.h"

namespace srsgnb {

class prach_buffer;
struct prach_buffer_context;
class resource_grid;
struct resource_grid_context;
class resource_grid_reader;

/// RU uplink received symbol context.
struct ru_up_rx_symbol_context {
  /// Slot context.
  slot_point slot;
  /// Radio sector identifier.
  unsigned sector;
  /// The last processed symbol index within the slot.
  unsigned symbol_id;
};

/// \brief Radio Unit uplink plane request handler.
///
/// Handles PRACH and uplink data requests.
class ru_up_request_handler
{
public:
  virtual ~ru_up_request_handler() = default;

  /// \brief Requests the RU to capture a PRACH window.
  ///
  /// The RU must capture a PHY window identified by \c context.
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

/// Radio Unit uplink plane notifier.
class ru_up_notifier
{
public:
  virtual ~ru_up_notifier() = default;

  /// \brief Notifies the completion of an OFDM symbol for a given context.
  ///
  /// \param[in] context Notification context.
  /// \param[in] grid    Resource grid that belongs to the context.
  virtual void on_new_uplink_symbol(const ru_up_rx_symbol_context& context, const resource_grid_reader& grid) = 0;

  /// \brief Notifies the completion of PRACH window.
  ///
  /// The RU uses this method to notify that a PRACH window identified by \c context has been written in \c buffer.
  ///
  /// \param[in] context PRACH context.
  /// \param[in] buffer  Read-only PRACH buffer.
  virtual void on_new_prach_window_data(const prach_buffer_context& context, const prach_buffer& bufferr) = 0;
};

/// \brief Radio Unit uplink plane.
///
/// This interface provides access to the uplink plane functionality of the RU, allowing to retrieve the uplink request
/// handler and setting the uplink notifier.
class ru_uplink_plane
{
public:
  virtual ~ru_uplink_plane() = default;

  /// Returns the request handler of this RU uplink plane.
  virtual ru_up_request_handler& get_request_handler() = 0;

  /// \brief Sets the RU uplink notifier for this RU uplink plane.
  ///
  /// \param[in] notifier_ Notifier to set.
  virtual void set_ru_up_notifier(ru_up_notifier& notifier_) = 0;
};

} // namespace srsgnb