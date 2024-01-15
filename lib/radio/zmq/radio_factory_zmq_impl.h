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

#include "radio_config_zmq_validator.h"
#include "srsran/radio/radio_factory.h"

namespace srsran {

/// Describes a ZeroMQ Radio based factory.
class radio_factory_zmq_impl : public radio_factory
{
public:
  /// Default constructor.
  radio_factory_zmq_impl() = default;

  // See interface for documentation.
  const radio_configuration::validator& get_configuration_validator() override;

  // See interface for documentation.
  std::unique_ptr<radio_session> create(const radio_configuration::radio& config,
                                        task_executor&                    async_task_executor,
                                        radio_notification_handler&       notifier) override;

private:
  static radio_config_zmq_config_validator config_validator;
};

} // namespace srsran
