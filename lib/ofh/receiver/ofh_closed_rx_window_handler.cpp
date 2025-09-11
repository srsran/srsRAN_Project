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

#include "ofh_closed_rx_window_handler.h"
#include "srsran/support/executors/task_executor.h"

using namespace srsran;
using namespace ofh;

closed_rx_window_handler::closed_rx_window_handler(const closed_rx_window_handler_config&  config,
                                                   closed_rx_window_handler_dependencies&& dependencies) :
  notification_delay_in_symbols(config.nof_symbols_to_process_uplink + config.rx_timing_params.sym_end + 1),
  sector_id(config.sector),
  warn_unreceived_frames(config.warn_unreceived_frames),
  log_unreceived_messages(config.warn_unreceived_frames == warn_unreceived_ru_frames::always),
  logger(*dependencies.logger),
  executor(*dependencies.executor),
  prach_repo(std::move(dependencies.prach_repo)),
  uplink_repo(std::move(dependencies.uplink_repo)),
  notifier(std::move(dependencies.notifier))
{
  srsran_assert(prach_repo, "Invalid PRACH context repository");
  srsran_assert(uplink_repo, "Invalid uplink context repository");
  srsran_assert(notifier, "Invalid U-Plane received symbol notifier");
}

void closed_rx_window_handler::on_new_symbol(const slot_symbol_point_context& symbol_point_context)
{
  if (stop_manager.stop_was_requested()) {
    return;
  }

  if (!executor.defer([internal_slot = symbol_point_context.symbol_point - notification_delay_in_symbols,
                       this,
                       token = stop_manager.get_token()]() {
        // Add pending contexts to the repository.
        uplink_repo->process_pending_contexts();
        prach_repo->process_pending_contexts();

        // Check the repositories for unhandled contexts.
        handle_uplink_context(internal_slot);
        handle_prach_context(internal_slot);
      })) {
    logger.warning(
        "Sector#{}: failed to dispatch task for checking for lost messages in reception for slot '{}' and symbol '{}'",
        sector_id,
        symbol_point_context.symbol_point.get_slot(),
        symbol_point_context.symbol_point.get_symbol_index());
  }
}

void closed_rx_window_handler::collect_metrics(closed_rx_window_metrics& metrics)
{
  // Fill the metrics and reset internal counters.
  metrics.nof_missing_prach_contexts = nof_missed_prach_contexts.exchange(0, std::memory_order_relaxed);
  metrics.nof_missing_uplink_symbols = nof_missed_uplink_symbols.exchange(0, std::memory_order_relaxed);
}

void closed_rx_window_handler::handle_uplink_context(slot_symbol_point symbol_point)
{
  expected<uplink_context::uplink_context_resource_grid_info> context =
      uplink_repo->pop_resource_grid_symbol(symbol_point.get_slot(), symbol_point.get_symbol_index());

  if (!context) {
    return;
  }

  uplink_context::uplink_context_resource_grid_info& ctx_value = *context;

  uplane_rx_symbol_context notification_context = {
      ctx_value.context.slot, symbol_point.get_symbol_index(), ctx_value.context.sector};
  notifier->on_new_uplink_symbol(notification_context, std::move(ctx_value.grid), false);

  if (log_unreceived_messages) {
    // Increase the metrics counter.
    nof_missed_uplink_symbols.fetch_add(1, std::memory_order_relaxed);

    logger.warning("Sector#{}: missed incoming User-Plane uplink messages for slot '{}', symbol '{}'",
                   ctx_value.context.sector,
                   ctx_value.context.slot,
                   symbol_point.get_symbol_index());
  }

  if (SRSRAN_UNLIKELY(logger.debug.enabled())) {
    logger.debug("Sector#{}: notifying incomplete UL symbol in slot '{}', symbol '{}'",
                 notification_context.sector,
                 notification_context.slot,
                 notification_context.symbol);
  }
}

void closed_rx_window_handler::handle_prach_context(slot_symbol_point symbol_point)
{
  // As the PRACH is sent when all the symbols are received, wait until new slot to notify it PRACH buffer.
  if (symbol_point.get_symbol_index() != 0) {
    return;
  }

  slot_point slot    = symbol_point.get_slot() - 1;
  auto       context = prach_repo->pop_prach_buffer(slot);

  // Nothing to do.
  if (!context) {
    return;
  }

  const auto& ctx_value = *context;

  notifier->on_new_prach_window_data(ctx_value.context, *ctx_value.buffer);

  if (log_unreceived_messages) {
    nof_missed_prach_contexts.fetch_add(1, std::memory_order_relaxed);

    logger.warning("Sector#{}: missed incoming User-Plane PRACH messages for slot '{}'",
                   ctx_value.context.sector,
                   ctx_value.context.slot);
  }

  if (SRSRAN_UNLIKELY(logger.debug.enabled())) {
    logger.debug(
        "Sector#{}: notifying incomplete PRACH in slot '{}'", ctx_value.context.sector, ctx_value.context.slot);
  }
}
