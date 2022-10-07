/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "harq_process.h"
#include "srsgnb/mac/bsr_format.h"

namespace srsgnb {

class ul_logical_channel_manager
{
public:
  void set_status(lcg_id_t lcg_id, bool active) { groups[lcg_id].active = true; }

  /// \brief Verifies if logical channel group is activated for UL.
  bool is_active(lcg_id_t lcg_id) const { return groups[lcg_id].active; }

  /// \brief Checks whether a logical channel has pending data.
  bool has_pending_bytes() const
  {
    return std::any_of(groups.begin(), groups.end(), [](const auto& ch) { return ch.active and ch.buf_st > 0; });
  }

  /// \brief Calculates total number of UL bytes.
  unsigned pending_bytes() const
  {
    unsigned bytes = 0;
    for (unsigned i = 0; i <= MAX_LCG_ID; ++i) {
      bytes += pending_bytes(uint_to_lcg_id(i));
    }
    return bytes;
  }

  /// \brief Last UL BSR for given LCG-ID.
  unsigned pending_bytes(lcg_id_t lcg_id) const { return is_active(lcg_id) ? groups[lcg_id].buf_st : 0; }

  /// \brief Checks whether a SR indication handling is pending.
  bool has_pending_sr() const { return sr_pending; }

  /// \brief Update UL BSR for a given LCG-ID.
  void handle_bsr_indication(const ul_bsr_indication_message& msg)
  {
    for (const auto& lcg_report : msg.reported_lcgs) {
      groups[lcg_report.lcg_id].buf_st = lcg_report.nof_bytes;
    }
  }

  /// \brief Indicate that the UE requested an UL grant.
  void handle_sr_indication(const sr_indication_message& msg)
  {
    sr_pending = true;
    // TODO: handle SR indication content.
  }

  void reset_sr_indication() { sr_pending = false; }

private:
  struct channel_group_context {
    bool active = false;
    /// DL Buffer status of this logical channel.
    unsigned buf_st = 0;
  };
  bool sr_pending = false;

  std::array<channel_group_context, MAX_NOF_LCGS> groups;
};

} // namespace srsgnb