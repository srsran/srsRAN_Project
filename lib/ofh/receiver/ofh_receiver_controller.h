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

#include "ofh_closed_rx_window_handler.h"
#include "ofh_message_receiver_impl.h"
#include "srsran/ofh/ofh_controller.h"

namespace srsran {
namespace ofh {

/// Open Fronthaul receiver controller.
class receiver_controller : public operation_controller
{
  message_receiver&         msg_receiver;
  closed_rx_window_handler& window_handler;

public:
  receiver_controller(message_receiver& msg_receiver_, closed_rx_window_handler& window_handler_) :
    msg_receiver(msg_receiver_), window_handler(window_handler_)
  {
  }

  // See interface for documentation.
  void start() override;

  // See interface for documentation.
  void stop() override;
};

} // namespace ofh
} // namespace srsran
