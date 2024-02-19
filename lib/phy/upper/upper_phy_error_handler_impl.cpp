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

#include "upper_phy_error_handler_impl.h"

using namespace srsran;

namespace {
/// Dummy implementation of an upper PHY error notifier.
class upper_phy_error_notifier_dummy : public upper_phy_error_notifier
{
public:
  void on_late_downlink_message(slot_point dl_msg_slot) override {}
};
} // namespace

/// Dummy error notifier used for instantiating this error handler.
static upper_phy_error_notifier_dummy dummy_notifier;

upper_phy_error_handler_impl::upper_phy_error_handler_impl() : error_notifier(dummy_notifier) {}

void upper_phy_error_handler_impl::handle_late_downlink_message(slot_point slot)
{
  error_notifier.get().on_late_downlink_message(slot);
}
