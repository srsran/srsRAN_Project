/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
