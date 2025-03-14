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

#include "../config/logical_channel_list_config.h"
#include "../slicing/ran_slice_id.h"
#include "srsran/adt/intrusive_list.h"
#include "srsran/mac/mac_pdu_format.h"
#include "srsran/scheduler/scheduler_feedback_handler.h"
#include "srsran/support/math/moving_averager.h"

namespace srsran {

class ul_logical_channel_manager
{
public:
  ul_logical_channel_manager(subcarrier_spacing scs, logical_channel_config_list_ptr log_channels_configs);

  /// \brief Update the configurations of the provided lists of bearers.
  void configure(logical_channel_config_list_ptr log_channels_configs);

  /// Deactivate all logical channel groups, handling of SRs to prepare for UE removal.
  void deactivate();

  /// Assign a RAN slice to a logical channel group.
  void set_lcg_ran_slice(lcg_id_t lcgid, ran_slice_id_t slice_id);

  /// Detach logical channel group from previously set RAN slice.
  void reset_lcg_ran_slice(lcg_id_t lcgid);

  /// Deactivate RAN slice and detach the associated logical channel groups.
  void deregister_ran_slice(ran_slice_id_t slice_id);

  /// Determines whether a RAN slice has at least one bearer associated with it.
  bool has_slice(ran_slice_id_t slice_id) const
  {
    unsigned idx = slice_id.value();
    return idx < slice_lcgid_list_lookup.size() and not slice_lcgid_list_lookup[idx].empty();
  }

  /// Get the RAN slice ID associated with a logical channel group.
  std::optional<ran_slice_id_t> get_slice_id(lcg_id_t lcgid) const { return groups[lcgid].slice_id; }

  /// Signal the start of a new slot.
  void slot_indication();

  /// \brief Verifies if logical channel group is activated for UL.
  bool is_active(lcg_id_t lcg_id) const { return groups[lcg_id].active; }

  /// \brief Verifies if at least one logical channel group is active.
  bool is_active() const
  {
    return std::any_of(groups.begin(), groups.end(), [](const auto& g) { return g.active; });
  }

  /// \brief Checks whether a logical channel has pending data.
  bool has_pending_bytes() const
  {
    return std::any_of(groups.begin(), groups.end(), [](const auto& ch) { return ch.active and ch.buf_st > 0; });
  }

  /// \brief Calculates total number of UL bytes.
  unsigned pending_bytes() const;

  /// \brief Calculates total number of UL bytes for a given slice.
  unsigned pending_bytes(ran_slice_id_t slice_id) const;

  /// \brief Returns the last UL BSR for given LCG-ID plus the estimated number of bytes required for the upper layer
  /// headers and MAC subPDU subheader.
  unsigned pending_bytes(lcg_id_t lcg_id) const
  {
    // Note: TS38.321, 6.1.3.1 - The size of the RLC and MAC headers are not considered in the buffer size computation.
    return is_active(lcg_id) ? get_mac_sdu_required_bytes(add_upper_layer_header_bytes(lcg_id, groups[lcg_id].buf_st))
                             : 0;
  }

  /// \brief Checks whether a SR indication handling is pending.
  bool has_pending_sr() const { return sr_pending.load(std::memory_order_relaxed); }

  /// \brief Update UL BSR for a given LCG-ID.
  void handle_bsr_indication(const ul_bsr_indication_message& msg)
  {
    for (const auto& lcg_report : msg.reported_lcgs) {
      groups[lcg_report.lcg_id].buf_st            = lcg_report.nof_bytes;
      groups[lcg_report.lcg_id].sched_bytes_accum = 0;
    }
  }

  /// \brief Indicate that the UE requested an UL grant.
  void handle_sr_indication()
  {
    if (not is_active()) {
      // Ignore SR indication if the UL has been deactivated.
      return;
    }
    sr_pending.store(true, std::memory_order::memory_order_relaxed);
    // TODO: handle SR indication content.
  }

  void reset_sr_indication() { sr_pending.store(false, std::memory_order::memory_order_relaxed); }

  /// \brief Average bit rate, in bps, for a given LCG-Id.
  double average_bit_rate(lcg_id_t lcg_id) const
  {
    return is_active(lcg_id) and groups[lcg_id].avg_bytes_per_slot.size() > 0
               ? groups[lcg_id].avg_bytes_per_slot.average() * 8 * slots_per_sec
               : 0.0;
  }

  /// \brief Register the scheduling of an UL grant for this UE.
  ///
  /// This event will be used to update estimated bit rates.
  void handle_ul_grant(unsigned grant_size);

private:
  struct channel_group_context : public intrusive_double_linked_list_element<> {
    bool active = false;
    /// DL Buffer status of this logical channel.
    unsigned buf_st = 0;
    /// Bytes-per-slot average for this logical channel.
    moving_averager<unsigned> avg_bytes_per_slot;
    /// Last slot sched bytes.
    unsigned last_sched_bytes = 0;
    /// Sched bytes since last BSR.
    unsigned sched_bytes_accum = 0;
    /// Slice associated with this channel.
    std::optional<ran_slice_id_t> slice_id;
  };

  /// \brief Adds an estimate of the upper layer required header bytes.
  static unsigned add_upper_layer_header_bytes(lcg_id_t lcgid, unsigned payload_bytes)
  {
    // Estimate of the number of bytes required for the upper layer header.
    static constexpr unsigned RLC_HEADER_SIZE_ESTIMATE = 3U;

    if (payload_bytes == 0 or lcgid == 0) {
      // In case of no payload or LCG-ID == 0, there is no need to account for upper layer header.
      return payload_bytes;
    }
    return payload_bytes + RLC_HEADER_SIZE_ESTIMATE;
  }

  // Number of slots per second given the used SCS. Parameter used to compute bit rates.
  const unsigned slots_per_sec;

  // List of UE-dedicated logical channel configurations.
  logical_channel_config_list_ptr lc_channels_configs;

  // This state variable tells whether there is a pending SR. Note: It is an atomic variable because SR indications
  // can be received from different cells (in different threads).
  std::atomic<bool> sr_pending{false};

  std::array<channel_group_context, MAX_NOF_LCGS> groups;

  // Mapping of RAN slice ID to the list of associated LCG-IDs.
  std::vector<intrusive_double_linked_list<channel_group_context>> slice_lcgid_list_lookup;
};

} // namespace srsran
