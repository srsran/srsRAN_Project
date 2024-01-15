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

struct config_request;
struct param_request;
struct start_request;
struct stop_request;

/// This interface represents the gateway for sending FAPI configuration messages to the underlying PHY.
class config_message_gateway
{
public:
  virtual ~config_message_gateway() = default;

  /// \brief Sends a param request message.
  ///
  /// \param[in]  msg   Message contents.
  virtual void param_request(const param_request& msg) = 0;

  /// \brief Sends a configuration request message.
  ///
  /// \param[in]  msg   Message contents.
  virtual void config_request(const config_request& msg) = 0;

  /// \brief Sends a start request message.
  ///
  /// \param[in]  msg   Message contents.
  virtual void start_request(const start_request& msg) = 0;

  /// \brief Sends a stop request message.
  ///
  /// \param[in]  msg   Message contents.
  virtual void stop_request(const stop_request& msg) = 0;
};

} // namespace fapi
} // namespace srsran
