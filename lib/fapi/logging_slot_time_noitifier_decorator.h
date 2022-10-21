/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/fapi/message_loggers.h"
#include "srsgnb/fapi/slot_time_message_notifier.h"
#include "srsgnb/srslog/srslog.h"

namespace srsgnb {
namespace fapi {

class logging_slot_time_notifier_decorator : public slot_time_message_notifier
{
public:
  logging_slot_time_notifier_decorator(srslog::basic_logger& logger, slot_time_message_notifier& notifier) :
    logger(logger), notifier(notifier)
  {
  }

  // See interface for documentation.
  void on_slot_indication(const slot_indication_message& msg) override;

private:
  /// FAPI logger.
  srslog::basic_logger&       logger;
  slot_time_message_notifier& notifier;
};

} // namespace fapi
} // namespace srsgnb