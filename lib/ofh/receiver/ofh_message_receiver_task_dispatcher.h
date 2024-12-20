/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "ofh_message_receiver.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/executors/task_executor.h"

namespace srsran {
namespace ofh {

/// Open Fronthaul message receiver interface implementation that dispatches tasks to the actual receiver.
class ofh_message_receiver_task_dispatcher : public message_receiver
{
public:
  ofh_message_receiver_task_dispatcher(message_receiver& msg_receiver_, task_executor& executor_, unsigned sector_) :
    msg_receiver(msg_receiver_), executor(executor_), sector(sector_)
  {
  }

  // See interface for documentation.
  void on_new_frame(ether::unique_rx_buffer buffer) override
  {
    if (!executor.execute([this, buff = std::move(buffer)]() mutable { msg_receiver.on_new_frame(std::move(buff)); })) {
      srslog::fetch_basic_logger("OFH").warning("Failed to dispatch receiver task for sector#{}", sector);
    }
  }

  // See interface for documentation.
  ether::receiver& get_ethernet_receiver() override { return msg_receiver.get_ethernet_receiver(); }

private:
  message_receiver& msg_receiver;
  task_executor&    executor;
  const unsigned    sector;
};

} // namespace ofh
} // namespace srsran
