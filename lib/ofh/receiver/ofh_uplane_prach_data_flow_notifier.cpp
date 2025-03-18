/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ofh_uplane_prach_data_flow_notifier.h"
#include "srsran/ofh/ofh_uplane_rx_symbol_notifier.h"

using namespace srsran;
using namespace ofh;

void uplane_prach_data_flow_notifier::notify_prach(slot_point slot)
{
  expected<prach_context::prach_context_information> context =
      prach_context_repo->try_poping_complete_prach_buffer(slot);

  if (!context.has_value()) {
    return;
  }

  const auto& ctx_value = context.value();
  notifier->on_new_prach_window_data(ctx_value.context, *ctx_value.buffer);

  if (SRSRAN_UNLIKELY(logger.debug.enabled())) {
    logger.debug("Notifying PRACH in slot '{}' for sector#{}", ctx_value.context.slot, ctx_value.context.sector);
  }
}
