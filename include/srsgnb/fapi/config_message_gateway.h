/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_FAPI_CONFIG_MESSAGE_GATEWAY_H
#define SRSGNB_FAPI_CONFIG_MESSAGE_GATEWAY_H

namespace srsgnb {
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
} // namespace srsgnb

#endif // SRSGNB_FAPI_CONFIG_MESSAGE_GATEWAY_H
