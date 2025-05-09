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
class slot_error_message_notifier;

/// \brief FAPI configuration message interface collection interface.
///
/// This interface gives access to the FAPI configuration message interfaces.
class fapi_config_message_interface_collection
{
public:
  virtual ~fapi_config_message_interface_collection() = default;

  /// Returns the config message gateway of this FAPI cell configurator.
  virtual config_message_gateway& get_config_message_gateway() = 0;

  /// Sets the config message notifier of this FAPI cell configurator.
  virtual void set_config_message_notifier(config_message_notifier& config_notifier) = 0;

  /// Sets the error message notifier of this FAPI cell configurator.
  virtual void set_slot_error_message_notifier(slot_error_message_notifier& err_notifier) = 0;

  /// Sets the cell operation request notifier of this FAPI cell configurator.
  virtual void set_cell_operation_request_notifier(cell_operation_request_notifier& cell_notifier) = 0;
};

} // namespace fapi
} // namespace srsran
