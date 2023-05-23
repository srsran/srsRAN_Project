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

#include "srsran/ofh/ofh_timing_notifier.h"
#include "srsran/ru/ru_timing_notifier.h"

namespace srsran {

/// RU timing handler for the Open Fronthaul interface.
class ru_ofh_timing_handler : public ofh::timing_notifier
{
public:
  explicit ru_ofh_timing_handler(ru_timing_notifier& notifier_) : notifier(notifier_) {}

  // See interface for documentation.
  void on_tti_boundary(slot_point slot) override { notifier.on_tti_boundary(slot); }

private:
  ru_timing_notifier& notifier;
};

} // namespace srsran
