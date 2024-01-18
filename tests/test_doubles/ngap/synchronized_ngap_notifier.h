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

#include "srsran/adt/concurrent_queue.h"
#include "srsran/ngap/ngap.h"
#include "srsran/ngap/ngap_message.h"

namespace srsran {
namespace srs_cu_cp {

class synchronized_dummy_ngap_message_notifier : public ngap_message_notifier
{
public:
  explicit synchronized_dummy_ngap_message_notifier(unsigned queue_size = 1024) : queue(queue_size) {}

  void on_new_message(const ngap_message& msg) override
  {
    bool success = queue.push_blocking(msg);
    report_error_if_not(success, "Queue is full");
  }

  bool try_pop_pdu(ngap_message& pdu) { return queue.try_pop(pdu); }

  bool pop_blocking_pdu(ngap_message& pdu) { return queue.pop_blocking(pdu); }

private:
  concurrent_queue<ngap_message,
                   concurrent_queue_policy::locking_mpmc,
                   concurrent_queue_wait_policy::condition_variable>
      queue;
};

} // namespace srs_cu_cp
} // namespace srsran