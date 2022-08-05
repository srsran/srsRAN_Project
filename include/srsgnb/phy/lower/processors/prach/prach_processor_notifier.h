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
class prach_buffer;

/// \brief Lower physical layer PRACH processor - Notifier interface.
///
/// Notifies a variety of PRACH-related events such as requests outside the occasion window or overflows of PRACH
/// requests.
class prach_processor_notifier
{
public:
  /// Default destructor.
  virtual ~prach_processor_notifier() = default;

  /// \brief Notifies a PRACH request outside the slot window.
  ///
  /// See \ref lower_phy_error_notifier::on_prach_request_late for more information.
  /// \param[in] context PRACH context.
  virtual void on_prach_request_late(const prach_buffer_context& context) = 0;

  /// \brief Notifies an excess of PRACH requests.
  ///
  /// See \ref lower_phy_error_notifier::on_prach_request_late for more information.
  /// \param[in] context PRACH context.
  virtual void on_prach_request_overflow(const prach_buffer_context& context) = 0;

  /// \brief Notifies the completion of the PRACH request processing.
  ///
  /// See \ref lower_phy_error_notifier::on_prach_request_late for more information.
  /// \param[in] buffer  PRACH buffer.
  /// \param[in] context PRACH context.
  virtual void on_rx_prach_window(const prach_buffer& buffer, const prach_buffer_context& context) = 0;
};

} // namespace srsgnb