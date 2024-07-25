/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ofh_closed_rx_window_handler.h"
#include "srsran/support/executors/task_executor.h"

using namespace srsran;
using namespace ofh;

closed_rx_window_handler::closed_rx_window_handler(const closed_rx_window_handler_config& config,
                                                   closed_rx_window_handler_dependencies  dependencies) :
  notification_delay_in_symbols(config.nof_symbols_to_process_uplink + config.rx_timing_params.sym_end + 1),
  log_unreceived_messages(config.warn_unreceived_ru_frames),
  logger(*dependencies.logger),
  executor(*dependencies.executor),
  prach_repo(std::move(dependencies.prach_repo)),
  uplink_repo(std::move(dependencies.uplink_repo)),
  notifier(std::move(dependencies.notifier))
{
  srsran_assert(dependencies.logger, "Invalid logger");
  srsran_assert(dependencies.executor, "Invalid executor");
  srsran_assert(prach_repo, "Invalid PRACH context repository");
  srsran_assert(uplink_repo, "Invalid uplink context repository");
  srsran_assert(notifier, "Invalid U-Plane received symbol notifier");
}

void closed_rx_window_handler::on_new_symbol(slot_symbol_point slot_symbol)
{
  if (!executor.defer([slot_symbol, this]() {
        // Use an internal slot symbol to decide the context to notify.
        slot_symbol_point internal_slot = slot_symbol - notification_delay_in_symbols;
        handle_uplink(internal_slot);
        handle_prach(internal_slot);
      })) {
    logger.warning("Failed to dispatch checking for lost messages in reception for slot '{}' and symbol '{}'",
                   slot_symbol.get_slot(),
                   slot_symbol.get_symbol_index());
  }
}

void closed_rx_window_handler::handle_uplink(slot_symbol_point slot_symbol)
{
  const auto& context =
      uplink_repo->pop_complete_resource_grid_symbol(slot_symbol.get_slot(), slot_symbol.get_symbol_index());

  if (!context.has_value()) {
    return;
  }

  const auto               ctx_value            = context.value();
  uplane_rx_symbol_context notification_context = {
      ctx_value.context.slot, slot_symbol.get_symbol_index(), ctx_value.context.sector};
  notifier->on_new_uplink_symbol(notification_context, ctx_value.grid->get_reader());

  // Log unreceived messages.
  if (log_unreceived_messages) {
    logger.warning("Missed incoming User-Plane uplink messages for slot '{}', symbol '{}' and sector#{}",
                   ctx_value.context.slot,
                   slot_symbol.get_symbol_index(),
                   ctx_value.context.sector);
  }

  logger.debug("Notifying UL symbol in slot '{}', symbol '{}' for sector#{}",
               notification_context.slot,
               notification_context.symbol,
               notification_context.sector);
}

void closed_rx_window_handler::handle_prach(slot_symbol_point slot_symbol)
{
  // As the PRACH is send when all the symbols are received, wait until new slot to notify it PRACH buffer.
  if (slot_symbol.get_symbol_index() != 0) {
    return;
  }

  slot_point  slot    = slot_symbol.get_slot() - 1;
  const auto& context = prach_repo->pop_prach_buffer(slot);

  // Nothing to do.
  if (!context.has_value()) {
    return;
  }

  const auto& ctx_value = context.value<>();

  notifier->on_new_prach_window_data(ctx_value.context, *ctx_value.buffer);

  if (log_unreceived_messages) {
    logger.warning("Missed incoming User-Plane PRACH messages for slot '{}' and sector#{}",
                   ctx_value.context.sector,
                   ctx_value.context.sector);
  }

  logger.debug("Notifying PRACH in slot '{}' for sector#{}", ctx_value.context.slot, ctx_value.context.sector);
}