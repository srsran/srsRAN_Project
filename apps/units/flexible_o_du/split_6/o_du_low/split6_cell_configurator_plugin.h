/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

class config_message_notifier;
class slot_error_message_notifier;

} // namespace fapi

/// Split 6 cell configurator plugin interface.
class split6_cell_configurator_plugin
{
public:
  virtual ~split6_cell_configurator_plugin() = default;

  /// Returns the FAPI configuration message notifier of this plugin.
  virtual fapi::config_message_notifier& get_config_message_notifier() = 0;

  /// Returns the FAPI slot error message notifier of this plugin.
  virtual fapi::slot_error_message_notifier& get_slot_error_message_notifier() = 0;
};

} // namespace srsran
