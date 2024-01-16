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

#include "radio_factory_zmq_impl.h"
#include "radio_config_zmq_validator.h"
#include "radio_session_zmq_impl.h"

using namespace srsran;

std::unique_ptr<radio_session> radio_factory_zmq_impl::create(const radio_configuration::radio& config,
                                                              task_executor&                    async_task_executor,
                                                              radio_notification_handler&       notifier)
{
  // Create radio session based on ZMQ.
  std::unique_ptr<radio_session_zmq_impl> session =
      std::make_unique<radio_session_zmq_impl>(config, async_task_executor, notifier);

  // Check session was created successfully.
  if (!session->is_successful()) {
    return nullptr;
  }

  // Otherwise, return the instance.
  return session;
}

radio_config_zmq_config_validator srsran::radio_factory_zmq_impl::config_validator;

const radio_configuration::validator& radio_factory_zmq_impl::get_configuration_validator()
{
  return config_validator;
}
