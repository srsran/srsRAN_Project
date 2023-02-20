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

#include "radio_config_zmq_validator.h"
#include "srsgnb/radio/radio_factory.h"

namespace srsgnb {

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

} // namespace srsgnb
