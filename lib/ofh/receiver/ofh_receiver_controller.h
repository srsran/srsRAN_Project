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

#include "ofh_message_receiver.h"
#include "srsran/ofh/ofh_controller.h"

namespace srsran {
namespace ofh {

/// Open Fronthaul receiver controller.
class receiver_controller : public operation_controller
{
  message_receiver& msg_receiver;

public:
  explicit receiver_controller(message_receiver& msg_receiver_) : msg_receiver(msg_receiver_) {}

  // See interface for documentation.
  void start() override;

  // See interface for documentation.
  void stop() override;
};

} // namespace ofh
} // namespace srsran
