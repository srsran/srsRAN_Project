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

#include "mac_cell_time_mapper_impl.h"
#include "srsran/srslog/srslog.h"

using namespace srsran;

atomic_sfn_time_mapper::atomic_sfn_time_mapper(unsigned numerology_) :
  numerology(numerology_), nof_slots_per_sf(get_nof_slots_per_subframe(to_subcarrier_spacing(numerology_)))
{
}

void atomic_sfn_time_mapper::store(slot_point slot, time_point time)
{
  if (slot.subframe_slot_index() != 0) {
    return;
  }
  uint64_t packed_mapping = pack_sfn_time_mapping(slot, time);
  mapping.store(packed_mapping, std::memory_order_relaxed);
}

mac_cell_slot_time_info atomic_sfn_time_mapper::load(time_point now) const
{
  uint64_t packed_mapping = mapping.load(std::memory_order_relaxed);
  if (packed_mapping == 0) {
    return mac_cell_slot_time_info{};
  }
  return {load_sfn(packed_mapping, numerology, nof_slots_per_sf), load_time(packed_mapping, now)};
}

uint64_t atomic_sfn_time_mapper::pack_sfn_time_mapping(slot_point slot, time_point time)
{
  uint64_t ns_since_epoch    = std::chrono::duration_cast<std::chrono::nanoseconds>(time.time_since_epoch()).count();
  uint64_t truncated_ts_ns   = ns_since_epoch & timestamp_mask;
  uint64_t packed_sfn_sf_idx = slot.sfn() << 4 | slot.subframe_index();
  return (packed_sfn_sf_idx << timestamp_bits) | truncated_ts_ns;
}

slot_point atomic_sfn_time_mapper::load_sfn(uint64_t packed_mapping, unsigned numerology, unsigned nof_slots_per_sf)
{
  uint64_t packed_sfn_sf_idx = (packed_mapping & sfn_mask) >> timestamp_bits;
  uint64_t sf_idx            = packed_sfn_sf_idx & 0xf;
  uint64_t sfn_val           = packed_sfn_sf_idx >> 4;
  return slot_point(numerology, sfn_val, sf_idx * nof_slots_per_sf);
}

atomic_sfn_time_mapper::time_point atomic_sfn_time_mapper::load_time(uint64_t packed_mapping, time_point now)
{
  uint64_t truncated_ts_ns = (packed_mapping & timestamp_mask);
  uint64_t now_ns          = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
  uint64_t base            = now_ns & ~timestamp_mask; // Align now to period boundary.
  uint64_t candidate       = base + truncated_ts_ns;   // Initial timestamp reconstruction.

  // Adjust if the reconstructed timestamp is more than half a period away from now.
  if (candidate > now_ns + period / 2) {
    candidate -= period;
  } else if (candidate < now_ns - period / 2) {
    candidate += period;
  }

  return time_point(std::chrono::nanoseconds(candidate));
}

mac_cell_time_mapper_impl::mac_cell_time_mapper_impl(unsigned numerology_) :
  logger(srslog::fetch_basic_logger("MAC")),
  cur_slot_time_mapping(atomic_sfn_time_mapper(numerology_)),
  numerology(numerology_),
  slot_dur(usecs{1000U >> numerology_})
{
}

void mac_cell_time_mapper_impl::handle_slot_indication(const mac_cell_timing_context& context)
{
  if (context.sl_tx.subframe_slot_index() == 0) {
    cur_slot_time_mapping.store(context.sl_tx, context.time_point);
  }
}

std::optional<mac_cell_slot_time_info> mac_cell_time_mapper_impl::get_last_mapping() const
{
  const auto& last = cur_slot_time_mapping.load();
  if (SRSRAN_UNLIKELY(not last.sl_tx.valid())) {
    logger.warning("Slot point to time point mapping not available.");
    return std::nullopt;
  }

  return mac_cell_slot_time_info{last.sl_tx, last.time_point};
}

std::optional<mac_cell_time_mapper::time_point> mac_cell_time_mapper_impl::get_time_point(slot_point slot) const
{
  const auto& last = cur_slot_time_mapping.load();
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
  const auto& last = cur_slot_time_mapping.load();
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
