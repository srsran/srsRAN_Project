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

#include "srsgnb/mac/lcid_dl_sch.h"
#include "srsgnb/scheduler/scheduler_slot_handler.h"
#include <queue>

namespace srsgnb {

class dl_logical_channel_manager
{
  static constexpr unsigned MAC_MAX_SDU_SUBHEADER_SIZE = 3;

public:
  void set_status(lcid_t lcid, bool active) { channels[lcid].active = active; }

  /// \brief Verifies if logical channel is activated for DL.
  bool is_active(lcid_t lcid) const { return channels[lcid].active; }

  /// \brief Checks whether a logical channel has pending data.
  bool has_pending_bytes() const
  {
    return not pending_ces.empty() or
           std::any_of(channels.begin(), channels.end(), [](const auto& ch) { return ch.buf_st > 0; });
  }

  /// \brief Checks whether UE has pending CEs to be scheduled.
  bool has_pending_ces() const { return not pending_ces.empty(); }

  /// \brief Calculates total number of DL bytes, including MAC header overhead.
  unsigned pending_bytes() const
  {
    unsigned bytes = 0;
    for (const lcid_dl_sch_t& ce : pending_ces) {
      bytes += ce.sizeof_ce();
    }
    for (const channel_context& ch : channels) {
      if (ch.active) {
        bytes += ch.buf_st + MAC_MAX_SDU_SUBHEADER_SIZE;
      }
    }
    return bytes;
  }

  /// \brief Last DL buffer status for given LCID (no MAC overhead included).
  unsigned pending_bytes(lcid_t lcid) const { return is_active(lcid) ? channels[lcid].buf_st : 0; }

  /// \brief Update DL buffer status for a given LCID.
  void handle_dl_buffer_status_indication(lcid_t lcid, unsigned buffer_status)
  {
    channels[lcid].buf_st = buffer_status;
  }

  /// \brief Enqueue new MAC CE to be scheduled.
  void handle_mac_ce_indication(lcid_dl_sch_t ce)
  {
    // Give priority to CON RES ID over other CEs.
    if (ce == lcid_dl_sch_t::UE_CON_RES_ID) {
      pending_ces.push_front(ce);
    } else {
      pending_ces.push_back(ce);
    }
  }

  /// \brief Allocates highest priority MAC SDU within space of \c rem_bytes bytes. Updates \c lch_info with allocated
  /// bytes for the MAC SDU (no MAC subheader).
  /// \return Allocated bytes for MAC SDU (with subheader).
  unsigned allocate_mac_sdu(dl_msg_lc_info& lch_info, unsigned rem_bytes);

  /// \brief Allocates next MAC CE within space of \c rem_bytes bytes. Updates \c lch_info with allocated bytes for the
  /// MAC CE.
  /// \return Allocated bytes for MAC CE (with subheader).
  unsigned allocate_mac_ce(dl_msg_lc_info& lch_info, unsigned rem_bytes);

private:
  struct channel_context {
    bool active = false;
    /// DL Buffer status of this logical channel.
    unsigned buf_st = 0;
  };

  /// \brief Returns the next highest priority LCID.
  lcid_t get_max_prio_lcid() const;

  /// \brief Updates DL Buffer State for a given LCID based on available space.
  unsigned allocate_mac_sdu(lcid_t lcid, unsigned rem_bytes);

  std::array<channel_context, MAX_LCID> channels;

  std::deque<lcid_dl_sch_t> pending_ces;
};

/// \brief Allocate MAC SDUs and corresponding MAC subPDU subheaders.
/// \param[in] tb_info TB on which MAC subPDUs will be stored.
/// \param[in] lch_mng UE DL logical channel manager.
/// \param[in] total_tbs available space in bytes for subPDUs.
/// \return Total number of bytes allocated (including MAC subheaders).
unsigned allocate_mac_sdus(dl_msg_tb_info& tb_info, dl_logical_channel_manager& lch_mng, unsigned total_tbs);

/// \brief Allocate MAC subPDUs for pending MAC CEs.
/// \param[in] tb_info TB on which MAC subPDUs will be stored.
/// \param[in] lch_mng UE DL logical channel manager.
/// \param[in] total_tbs available space in bytes for subPDUs.
/// \return Total number of bytes allocated (including MAC subheaders).
unsigned allocate_mac_ces(dl_msg_tb_info& tb_info, dl_logical_channel_manager& lch_mng, unsigned total_tbs);

} // namespace srsgnb