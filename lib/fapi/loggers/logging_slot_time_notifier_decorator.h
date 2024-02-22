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

#include "srsran/fapi/slot_time_message_notifier.h"
#include "srsran/srslog/srslog.h"

namespace srsran {
namespace fapi {

/// Adds logging information over the implemented interface.
class logging_slot_time_notifier_decorator : public slot_time_message_notifier
{
public:
  logging_slot_time_notifier_decorator(srslog::basic_logger& logger_, slot_time_message_notifier& notifier_) :
    logger(logger_), notifier(notifier_)
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
} // namespace srsran
