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

#include "srsran/mac/mac_cell_manager.h"
#include "srsran/mac/mac_cell_slot_handler.h"
#include "srsran/mac/mac_cell_timing_context.h"
#include "srsran/srslog/logger.h"

namespace srsran {

/// \brief A lock-free atomic mapper that stores System Frame Number (SFN) and system time.
/// This class provides an efficient, atomic way to store and retrieve a mapping between SFN and system time, using
/// a single 64-bit atomic variable. The mapping is updated every 1ms (every full subframe) when storing only the lower
/// 50 bits of the timestamp (nanoseconds since epoch). The full timestamp is recovered using
/// std::chrono::system_clock::now(), leveraging the fact that the timestamp wraps around roughly every 13 days and
/// that the difference between now() and the stored timestamp is smaller than 1 ms.
class atomic_sfn_time_mapper
{
public:
  using time_point = std::chrono::time_point<std::chrono::system_clock>;

  explicit atomic_sfn_time_mapper(unsigned numerology_);

  /// \brief Atomically stores the SFN and the associated timestamp.
  /// \param slot The slot point containing SFN and slot index.
  /// \param time The current system time to be stored.
  void store(slot_point slot, time_point time);

  /// \brief Loads the last stored SFN and timestamp atomically.
  /// \param  now The current system time, defaults to `std::chrono::system_clock::now()`.
  /// \return A structure containing the last stored SFN and timestamp.
  mac_cell_slot_time_info load(time_point now = std::chrono::system_clock::now()) const;

private:
  /// Number of bits used to store the timestamp.
  static constexpr uint64_t timestamp_bits = 50;
  /// Number of bits used to store the System Frame Number (10bits) and Sub-Frame Index (4bits).
  static constexpr uint64_t sfn_bits = 14;
  /// The period for timestamp wraparound (2^50 nanoseconds ≈ 13 days).
  static constexpr uint64_t period = 1ULL << timestamp_bits;
  /// Mask to extract the timestamp (lower 50 bits).
  static constexpr uint64_t timestamp_mask = (1ULL << timestamp_bits) - 1;
  /// Mask to extract the SFN (upper 14 bits).
  static constexpr uint64_t sfn_mask = ((1ULL << sfn_bits) - 1) << timestamp_bits;

  /// \brief Packs the SFN and system time (after truncation) into a 64-bit value.
  /// \param slot The slot point containing SFN and slot index.
  /// \param time The current system time to be stored.
  /// \return A 64-bit integer representing the packed SFN and truncated timestamp.
  static uint64_t pack_sfn_time_mapping(slot_point slot, time_point time);

  /// \brief Extracts the SFN from the packed 64-bit value.
  /// \param packed_mapping The packed SFN and truncated timestamp.
  /// \param numerology The numerology used for slot reconstruction.
  /// \param nof_slots_per_sf The nof slots in subframe used for slot reconstruction.
  /// \return The reconstructed slot_point.
  static slot_point load_sfn(uint64_t packed_mapping, unsigned numerology, unsigned nof_slots_per_sf);

  /// \brief Reconstructs the full timestamp from the packed 64-bit value.
  /// Since only the lower 50 bits of the timestamp are stored, this function reconstructs the full timestamp by
  /// leveraging the fact that timestamps change slowly (i.e., wrap around roughly every 13 days). It adjusts the
  /// truncated timestamp based on the current time to obtain the correct value.
  /// \param packed_mapping The packed SFN and truncated timestamp.
  /// \param now The current system time to assist in reconstruction.
  /// \return The reconstructed full timestamp.
  static time_point load_time(uint64_t packed_mapping, time_point now);

  /// The numerology parameter used for slot reconstruction.
  const unsigned numerology;

  /// Number of slots per system frame.
  const unsigned nof_slots_per_sf;

  /// Atomic storage for the packed SFN and truncated timestamp.
  std::atomic<uint64_t> mapping = {0};
};

/// \brief Class that provides mapping between slot point and time point.
/// This class provides a mechanism to track and convert between slot points (SFN, slot index) and time points (system
/// timestamps). It maintains an atomic reference SFN-Timestamp mapping updated every 1ms that is used for requested
/// conversion.
class mac_cell_time_mapper_impl final : public mac_cell_time_mapper
{
  using usecs = std::chrono::microseconds;

public:
  explicit mac_cell_time_mapper_impl(unsigned numerology_);

  /// \brief Handles a slot indication and updates the slot-to-time mapping.
  /// The mapping is stored only for the full sub-frames (i.e., every 1ms).
  /// \param context The MAC cell timing context containing slot and time information.
  void handle_slot_indication(const mac_cell_timing_context& context);

  /// \brief Retrieves the last known Slot_point-Time_point mapping.
  /// Note that only SFN-Timestamp are stored, hence it is updated every 1 ms.
  std::optional<mac_cell_slot_time_info> get_last_mapping() const override;

  /// Converts a given slot point to a corresponding time point.
  std::optional<time_point> get_time_point(slot_point slot) const override;

  /// Converts a given time point to a corresponding slot point.
  std::optional<slot_point> get_slot_point(time_point time) const override;

private:
  srslog::basic_logger& logger;

  /// Atomic SFN-Timestamp mapping used as reference to compute any requested slot_point-time_point mapping.
  ///  This is the core reference mapping that links a System Frame Number(SFN) and a timestamp.
  atomic_sfn_time_mapper cur_slot_time_mapping;

  /// Numerology parameter that determines slot duration and frame structure.
  const unsigned numerology;

  /// Duration of a single slot in microseconds.
  const usecs slot_dur;

  /// Duration of a full system frame (10 milliseconds).
  static constexpr usecs frame_dur = usecs{10000};
};

} // namespace srsran
