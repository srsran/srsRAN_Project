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

#pragma once

#include "rlf_detector.h"
#include "srsran/du_high/rnti_value_table.h"
#include "srsran/mac/mac.h"
#include "srsran/mac/mac_config.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/du_ue_list.h"
#include "srsran/scheduler/harq_id.h"
#include <mutex>

namespace srsran {

constexpr static size_t UE_CON_RES_ID_LEN = 6;
using ue_con_res_id_t                     = std::array<uint8_t, UE_CON_RES_ID_LEN>;

using du_rnti_table = rnti_value_table<du_ue_index_t, du_ue_index_t::INVALID_DU_UE_INDEX>;

class mac_dl_ue_manager
{
public:
  mac_dl_ue_manager(du_rnti_table& rnti_table_, rlf_detector& rlf_handler_);

  /// Check if UE with provided C-RNTI exists.
  /// \param rnti C-RNTI of the UE.
  /// \return True if UE exists. False, otherwise.
  bool contains_rnti(rnti_t rnti) const
  {
    du_ue_index_t ue_index = rnti_table[rnti];
    if (is_du_ue_index_valid(ue_index)) {
      std::lock_guard<std::mutex> lock(ue_mutex[ue_index]);
      return ue_db.contains(ue_index);
    }
    return false;
  }

  /// Checks if bearer with provided C-RNTI and LCID exists.
  bool contains_lcid(rnti_t rnti, lcid_t lcid) const
  {
    du_ue_index_t ue_index = rnti_table[rnti];
    if (is_du_ue_index_valid(ue_index)) {
      std::lock_guard<std::mutex> lock(ue_mutex[ue_index]);
      return ue_db.contains(ue_index) and ue_db[ue_index].dl_bearers.contains(lcid);
    }
    return false;
  }

  du_ue_index_t get_ue_index(rnti_t rnti) const
  {
    du_ue_index_t ue_index = rnti_table[rnti];
    if (not is_du_ue_index_valid(ue_index)) {
      return INVALID_DU_UE_INDEX;
    }
    return ue_index;
  }

  mac_sdu_tx_builder* get_bearer(rnti_t rnti, lcid_t lcid)
  {
    du_ue_index_t ue_index = rnti_table[rnti];
    if (not is_du_ue_index_valid(ue_index)) {
      return nullptr;
    }
    std::lock_guard<std::mutex> lock(ue_mutex[ue_index]);
    if (not ue_db.contains(ue_index)) {
      return nullptr;
    }
    ue_item& u = ue_db[ue_index];
    return u.dl_bearers.contains(lcid) ? u.dl_bearers[lcid] : nullptr;
  }

  bool add_ue(const mac_ue_create_request& request, std::vector<std::vector<uint8_t>> dl_harq_buffers);

  bool remove_ue(du_ue_index_t ue_index);

  bool addmod_bearers(du_ue_index_t ue_index, span<const mac_logical_channel_config> dl_logical_channels);

  bool remove_bearers(du_ue_index_t ue_index, span<const lcid_t> lcids);

  /// \brief Returns UE Contention Resolution Id, which is derived from Msg3 bytes.
  ue_con_res_id_t get_con_res_id(rnti_t rnti);

  span<uint8_t> get_dl_harq_buffer(rnti_t rnti, harq_id_t h_id, unsigned tb_idx)
  {
    du_ue_index_t               ue_index = rnti_table[rnti];
    std::lock_guard<std::mutex> lock(ue_mutex[ue_index]);
    if (not ue_db.contains(ue_index)) {
      return {};
    }
    return ue_db[ue_index].harq_buffers[h_id];
  }

  /// \brief Handle received UE CRC.
  void report_crc(du_ue_index_t ue_index, bool crc) { rlf_handler.handle_crc(ue_index, crc); }

  /// \brief Handle received UE HARQ-ACK.
  void report_ack(du_ue_index_t ue_index, bool ack) { rlf_handler.handle_ack(ue_index, ack); }

private:
  struct ue_item {
    du_ue_index_t                       ue_index = MAX_NOF_DU_UES;
    rnti_t                              rnti     = INVALID_RNTI;
    slotted_vector<mac_sdu_tx_builder*> dl_bearers;
    ue_con_res_id_t                     msg3_subpdu;
    std::vector<std::vector<uint8_t>>   harq_buffers;

    explicit ue_item(du_ue_index_t ue_index_, rnti_t rnti_) : ue_index(ue_index_), rnti(rnti_) {}
  };

  bool add_ue_nolock(du_ue_index_t                     ue_index,
                     rnti_t                            crnti,
                     const byte_buffer*                ul_ccch_msg,
                     std::vector<std::vector<uint8_t>> dl_harq_buffers,
                     mac_ue_radio_link_notifier&       rlf_notifier);

  bool addmod_bearers_nolock(du_ue_index_t ue_index, span<const mac_logical_channel_config> dl_logical_channels);

  du_rnti_table& rnti_table;
  rlf_detector&  rlf_handler;

  mutable std::array<std::mutex, MAX_NOF_DU_UES> ue_mutex;

  du_ue_list<ue_item> ue_db;
};

} // namespace srsran
