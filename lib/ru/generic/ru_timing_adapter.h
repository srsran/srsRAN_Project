/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "ru_radio_notifier_handler.h"
#include "srsran/phy/lower/lower_phy_timing_notifier.h"
#include "srsran/ru/ru_timing_notifier.h"

namespace srsran {

/// Implements a generic lower physical layer to Radio Unit timing adapter.
class ru_timing_adapter : public lower_phy_timing_notifier
{
public:
  ru_timing_adapter(ru_timing_notifier&                                    timing_handler_,
                    std::unique_ptr<ru_radio_notification_handler_counter> radio_event_counter_) :
    timing_handler(timing_handler_), radio_event_counter(std::move(radio_event_counter_))
  {
    srsran_assert(radio_event_counter, "Invalid radio event counter");
  }

  // See interface for documentation.
  void on_tti_boundary(const lower_phy_timing_context& context) override
  {
    timing_handler.on_tti_boundary(context.slot);
    radio_event_counter->print();
  }

  // See interface for documentation.
  void on_ul_half_slot_boundary(const lower_phy_timing_context& context) override
  {
    timing_handler.on_ul_half_slot_boundary(context.slot);
  }

  // See interface for documentation.
  void on_ul_full_slot_boundary(const lower_phy_timing_context& context) override
  {
    timing_handler.on_ul_full_slot_boundary(context.slot);
  }

private:
  ru_timing_notifier&                                    timing_handler;
  std::unique_ptr<ru_radio_notification_handler_counter> radio_event_counter;
};

} // namespace srsran
