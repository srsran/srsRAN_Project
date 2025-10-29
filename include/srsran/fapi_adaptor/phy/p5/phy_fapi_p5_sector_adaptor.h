/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

namespace srsran {

namespace fapi {
class cell_operation_request_notifier;
class config_message_gateway;
class config_message_notifier;
class error_message_notifier;
} // namespace fapi

namespace fapi_adaptor {

/// PHY-FAPI P5 sector adaptor.
class phy_fapi_p5_sector_adaptor
{
public:
  virtual ~phy_fapi_p5_sector_adaptor() = default;

  /// Returns the config message gateway of this FAPI cell configurator.
  virtual fapi::config_message_gateway& get_config_message_gateway() = 0;

  /// Sets the config message notifier of this FAPI cell configurator.
  virtual void set_config_message_notifier(fapi::config_message_notifier& config_notifier) = 0;

  /// Sets the error message notifier of this FAPI cell configurator.
  virtual void set_error_message_notifier(fapi::error_message_notifier& err_notifier) = 0;
};

} // namespace fapi_adaptor
} // namespace srsran
