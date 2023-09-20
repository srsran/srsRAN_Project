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

#include "ofh_uplane_prach_data_flow_notifier.h"
#include "srsran/ofh/ofh_uplane_rx_symbol_notifier.h"

using namespace srsran;
using namespace ofh;

void uplane_prach_data_flow_notifier::notify_prach(slot_point slot)
{
  expected<prach_context::prach_context_information> context =
      prach_context_repo.get(slot).try_getting_complete_prach_buffer();

  if (context.is_error()) {
    return;
  }

  // Clear the repository entry.
  prach_context_repo.clear(slot);

  const auto ctx_value = context.value();
  notifier.on_new_prach_window_data(ctx_value.context, *ctx_value.buffer);

  logger.debug("Notifying PRACH at slot={} and sector={}", ctx_value.context.slot, ctx_value.context.sector);
}
