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
namespace fapi {

struct config_response;
struct param_response;
struct stop_indication;

/// \brief Configuration message notifier interface.
///
/// This interface notifies the reception of FAPI configuration messages from the underlying PHY.
class config_message_notifier
{
public:
  virtual ~config_message_notifier() = default;

  /// \brief Notifies the reception of a parameter response message.
  ///
  /// \param[in] msg Message contents.
  virtual void on_param_response(const param_response& msg) = 0;

  /// \brief Notifies the reception of a configuration response message.
  ///
  /// \param[in] msg Message contents.
  virtual void on_config_response(const config_response& msg) = 0;

  /// \brief Notifies the reception of a stop response message.
  ///
  /// \param[in] msg Message contents.
  virtual void on_stop_indication(const stop_indication& msg) = 0;
};

} // namespace fapi
} // namespace srsran
