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
