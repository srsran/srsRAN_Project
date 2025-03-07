/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "mac_cell_time_mapper_impl.h"
#include "srsran/srslog/srslog.h"

using namespace srsran;

mac_cell_time_mapper_impl::mac_cell_time_mapper_impl(unsigned numerology_) :
  logger(srslog::fetch_basic_logger("MAC")),
  numerology(numerology_),
  nof_slots_per_sf(get_nof_slots_per_subframe(to_subcarrier_spacing(numerology_))),
  slot_dur(usecs{1000U >> numerology_})
{
}

void mac_cell_time_mapper_impl::handle_slot_indication(const mac_cell_timing_context& context)
{
  cur_slot_time_pair.write_and_commit(context);
}

std::optional<mac_cell_slot_time_info> mac_cell_time_mapper_impl::get_last_mapping() const
{
  const auto& last = cur_slot_time_pair.read();
  if (SRSRAN_UNLIKELY(not last.sl_tx.valid())) {
    logger.warning("Slot point to time point mapping not available.");
    return std::nullopt;
  }

  return mac_cell_slot_time_info{last.sl_tx, last.time_point};
}

std::optional<mac_cell_time_mapper::time_point> mac_cell_time_mapper_impl::get_time_point(slot_point slot) const
{
  const auto& last = cur_slot_time_pair.read();
  if (SRSRAN_UNLIKELY(not last.sl_tx.valid())) {
    logger.warning("Slot point to time point mapping not available");
    return std::nullopt;
  }

  if (not slot.valid()) {
    logger.warning("Cannot provide time point for the uninitialized slot point.");
    return std::nullopt;
  }

  if (slot.numerology() != numerology) {
    logger.warning("Cannot provide time point for a slot point with a different numerology.");
    return std::nullopt;
  }

  if (slot == last.sl_tx) {
    return last.time_point;
  }

  const auto time_difference = (slot - last.sl_tx) * slot_dur;
  return last.time_point + time_difference;
}

std::optional<slot_point> mac_cell_time_mapper_impl::get_slot_point(time_point time) const
{
  const auto& last = cur_slot_time_pair.read();
  if (SRSRAN_UNLIKELY(not last.sl_tx.valid())) {
    logger.warning("Slot point to time point mapping not available.");
    return std::nullopt;
  }

  if (time.time_since_epoch().count() == 0) {
    logger.warning("Cannot provide slot point for the uninitialized time point.");
    return std::nullopt;
  }

  const auto time_difference = std::chrono::duration_cast<std::chrono::microseconds>(time - last.time_point);
  const auto nof_slots       = time_difference.count() / slot_dur.count();
  return last.sl_tx + nof_slots;
}
