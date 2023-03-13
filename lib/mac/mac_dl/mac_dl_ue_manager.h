/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../du_rnti_table.h"
#include "srsran/mac/mac.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/du_ue_list.h"
#include "srsran/scheduler/harq_id.h"
#include <mutex>

namespace srsran {

constexpr static size_t UE_CON_RES_ID_LEN = 6;
using ue_con_res_id_t                     = std::array<uint8_t, UE_CON_RES_ID_LEN>;

class mac_dl_ue_manager
{
public:
  mac_dl_ue_manager(du_rnti_table& rnti_table_);

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

  bool add_ue(const mac_ue_create_request_message& request, std::vector<std::vector<uint8_t>> dl_harq_buffers);

  bool remove_ue(du_ue_index_t ue_index);

  bool addmod_bearers(du_ue_index_t ue_index, span<const mac_logical_channel_to_setup> dl_logical_channels);

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

private:
  struct ue_item {
    rnti_t                              rnti     = INVALID_RNTI;
    du_ue_index_t                       ue_index = MAX_NOF_DU_UES;
    slotted_vector<mac_sdu_tx_builder*> dl_bearers;
    ue_con_res_id_t                     msg3_subpdu;
    std::vector<std::vector<uint8_t>>   harq_buffers;
  };

  bool add_ue_nolock(du_ue_index_t                     ue_index,
                     rnti_t                            crnti,
                     const byte_buffer*                ul_ccch_msg,
                     std::vector<std::vector<uint8_t>> dl_harq_buffers);

  bool addmod_bearers_nolock(du_ue_index_t ue_index, span<const mac_logical_channel_to_setup> dl_logical_channels);

  du_rnti_table& rnti_table;

  mutable std::array<std::mutex, MAX_NOF_DU_UES> ue_mutex;

  du_ue_list<ue_item> ue_db;
};

} // namespace srsran
