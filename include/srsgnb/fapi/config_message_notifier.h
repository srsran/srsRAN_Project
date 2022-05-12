/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_FAPI_CONFIG_MESSAGE_NOTIFIER_H
#define SRSGNB_FAPI_CONFIG_MESSAGE_NOTIFIER_H

namespace srsgnb {
namespace fapi {

struct config_response;
struct param_response;
struct stop_indication;

/// This interface notifies the reception of FAPI configuration messages from the underlying PHY.
class config_message_notifier
{
public:
  virtual ~config_message_notifier() = default;

  /// \brief Callback to notify the reception of a parameter response message.
  ///
  /// \param[in]  msg   Message contents.
  virtual void on_param_response(const param_response& msg) = 0;

  /// \brief Callback to notify the reception of a configuration response message.
  ///
  /// \param[in]  msg   Message contents.
  virtual void on_config_response(const config_response& msg) = 0;

  /// \brief Callback to notify the reception of a stop response message.
  ///
  /// \param[in]  msg   Message contents.
  virtual void on_stop_indication(const stop_indication& msg) = 0;
};

} // namespace fapi
} // namespace srsgnb

#endif // SRSGNB_FAPI_CONFIG_MESSAGE_NOTIFIER_H
