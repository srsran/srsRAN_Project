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

#pragma once

#include "decorator_helpers/error_notifier_dispatcher.h"
#include "decorator_helpers/slot_data_message_notifier_dispatcher.h"
#include "decorator_helpers/slot_last_message_notifier_dispatcher.h"
#include "message_bufferer_slot_gateway_task_dispatcher.h"
#include "message_bufferer_slot_time_notifier_decorator.h"
#include "srsran/fapi/decorator.h"

namespace srsran {
namespace fapi {

/// \brief FAPI message bufferer decorator.
///
/// The message bufferer caches FAPI messages that are send by the L2 and forwards them to the L1 when the
/// SLOT.indication that matches the FAPI message arrives.
class message_bufferer_decorator_impl : public fapi_decorator
{
public:
  message_bufferer_decorator_impl(unsigned                    sector_id,
                                  unsigned                    l2_nof_slots_ahead,
                                  subcarrier_spacing          scs,
                                  slot_message_gateway&       gateway,
                                  slot_last_message_notifier& last_msg_notifier_,
                                  task_executor&              executor) :
    fapi_decorator({}),
    dispatcher(sector_id, l2_nof_slots_ahead, scs, gateway, executor),
    time_notifier(l2_nof_slots_ahead, scs, dispatcher)
  {
    last_msg_notifier.set_slot_last_message_notifier(last_msg_notifier_);
  }

  // See interface for documentation.
  slot_message_gateway& get_slot_message_gateway() override;

  // See interface for documentation.
  slot_last_message_notifier& get_slot_last_message_notifier() override;

  // See interface for documentation.
  void set_slot_time_message_notifier(slot_time_message_notifier& notifier) override;

  // See interface for documentation.
  void set_slot_data_message_notifier(slot_data_message_notifier& notifier) override;

  // See interface for documentation.
  void set_error_message_notifier(error_message_notifier& notifier) override;

private:
  // See interface for documentation.
  slot_time_message_notifier& get_slot_time_message_notifier_from_this_decorator() override;

  // See interface for documentation.
  slot_data_message_notifier& get_slot_data_message_notifier_from_this_decorator() override;

  // See interface for documentation.
  error_message_notifier& get_error_message_notifier_from_this_decorator() override;

private:
  message_bufferer_slot_gateway_task_dispatcher dispatcher;
  message_bufferer_slot_time_notifier_decorator time_notifier;
  slot_last_message_notifier_dispatcher         last_msg_notifier;
  slot_data_message_notifier_dispatcher         data_notifier;
  error_message_notifier_dispatcher             error_notifier;
};

} // namespace fapi
} // namespace srsran
