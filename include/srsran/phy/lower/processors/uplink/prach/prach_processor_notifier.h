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

namespace srsran {

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

} // namespace srsran