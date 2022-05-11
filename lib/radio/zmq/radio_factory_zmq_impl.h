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

#ifndef SRSGNB_LIB_RADIO_ZMQ_RADIO_FACTORY_ZMQ_IMPL_H
#define SRSGNB_LIB_RADIO_ZMQ_RADIO_FACTORY_ZMQ_IMPL_H

#include "srsgnb/radio/radio_factory.h"

namespace srsgnb {

/// Describes a ZeroMQ Radio based factory.
class radio_factory_zmq_impl : public radio_factory
{
public:
  /// Default constructor.
  radio_factory_zmq_impl() = default;

  // See interface for documentation.
  std::unique_ptr<radio_session> create(const radio_configuration::radio& config,
                                        task_executor&                    async_task_executor,
                                        radio_notification_handler&       notifier) override;
};

} // namespace srsgnb

#endif // SRSGNB_LIB_RADIO_ZMQ_RADIO_FACTORY_ZMQ_IMPL_H
