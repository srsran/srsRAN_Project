/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "slot_last_message_notifier_dispatcher.h"
#include "slot_last_message_notifier_dummy.h"

using namespace srsran;
using namespace fapi;

static slot_last_message_notifier_dummy dummy_notifier;

slot_last_message_notifier_dispatcher::slot_last_message_notifier_dispatcher() : notifier(&dummy_notifier) {}

void slot_last_message_notifier_dispatcher::on_last_message(slot_point slot)
{
  notifier->on_last_message(slot);
}

void slot_last_message_notifier_dispatcher::set_slot_last_message_notifier(
    slot_last_message_notifier& last_msg_notifier)
{
  notifier = &last_msg_notifier;
}
