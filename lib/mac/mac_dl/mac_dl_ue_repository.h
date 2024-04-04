/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "cell_dl_harq_buffer_pool.h"
#include "srsran/du_high/rnti_value_table.h"
#include "srsran/mac/mac.h"
#include "srsran/mac/mac_config.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/du_ue_list.h"
#include "srsran/scheduler/harq_id.h"

namespace srsran {

// Array of bytes used to store the UE Contention Resolution Id.
constexpr static size_t UE_CON_RES_ID_LEN = 6;
using ue_con_res_id_t                     = std::array<uint8_t, UE_CON_RES_ID_LEN>;

// Table for conversion between RNTI and ue indexes.
using du_rnti_table = rnti_value_table<du_ue_index_t, du_ue_index_t::INVALID_DU_UE_INDEX>;

/// Context of a UE in the MAC DL.
class mac_dl_ue_context
{
public:
  explicit mac_dl_ue_context(const mac_ue_create_request& req);
  // expensive copy due to HARQ buffers. Cheap move.
  mac_dl_ue_context(const mac_dl_ue_context&)                = delete;
  mac_dl_ue_context(mac_dl_ue_context&&) noexcept            = default;
  mac_dl_ue_context& operator=(const mac_dl_ue_context&)     = delete;
  mac_dl_ue_context& operator=(mac_dl_ue_context&&) noexcept = default;

  du_ue_index_t get_ue_index() const { return ue_index; }

  // DL Logical Channel methods.
  const slotted_id_vector<lcid_t, mac_sdu_tx_builder*>& logical_channels() const { return dl_bearers; }
  void addmod_logical_channels(span<const mac_logical_channel_config> dl_logical_channels);
  void remove_logical_channels(span<const lcid_t> lcids_to_remove);

  const ue_con_res_id_t& get_con_res_id() const { return msg3_subpdu; }

private:
  du_ue_index_t                                  ue_index;
  slotted_id_vector<lcid_t, mac_sdu_tx_builder*> dl_bearers;
  ue_con_res_id_t                                msg3_subpdu = {};
};

/// Repository used to map upper layer bearers to MAC DL-SCH logical channels.
class mac_dl_ue_repository
{
public:
  mac_dl_ue_repository(du_rnti_table& rnti_table_);

  /// Lookup UE index based on RNTI.
  du_ue_index_t get_ue_index(rnti_t rnti) const
  {
    du_ue_index_t ue_index = rnti_table[rnti];
    if (not is_du_ue_index_valid(ue_index)) {
      return INVALID_DU_UE_INDEX;
    }
    return ue_index;
  }

  /// \brief Get the MAC SDU builder for a specific UE and DL-SCH LCID.
  /// \param[in] rnti RNTI of the UE.
  /// \param[in] lcid DL-SCH Logical Channel ID.
  /// \return Pointer to the DL-SCH SDU builder. Nullptr if not found.
  mac_sdu_tx_builder* get_lc_sdu_builder(rnti_t rnti, lcid_t lcid)
  {
    // Lookup the UE index based on RNTI.
    du_ue_index_t ue_index = rnti_table[rnti];
    if (not is_du_ue_index_valid(ue_index) or not ue_db.contains(ue_index)) {
      return nullptr;
    }

    // Return the MAC SDU builder for the specified LCID.
    auto& u = ue_db[ue_index];
    return u.logical_channels().contains(lcid) ? u.logical_channels()[lcid] : nullptr;
  }

  /// \brief Add a new UE context to the mapper.
  /// \param[in] ue_to_add UE context to add.
  /// \return true if successfully added. False, otherwise.
  bool add_ue(mac_dl_ue_context ue_to_add);

  /// \brief Remove a UE context from the mapper and its associated bearers.
  /// \param[in] ue_index UE index to remove.
  /// \return true if successfully removed. False, if UE did not exist.
  bool remove_ue(du_ue_index_t ue_index);

  /// \brief Add/Modify bearers for an existing UE.
  /// \param[in] ue_index UE index for which to add/modify bearers.
  /// \param[in] dl_logical_channels New config of the logical channels to add/modify.
  /// \return true if successfully modified. False, if the UE does not exist.
  bool addmod_bearers(du_ue_index_t ue_index, span<const mac_logical_channel_config> dl_logical_channels);

  /// \brief Remove specified bearers for an existing UE.
  /// \param[in] ue_index UE index for which to remove bearers.
  /// \param[in] lcids LCIDs of the bearers to remove.
  /// \return true if successfully removed. False, if the UE or bearers do not exist.
  bool remove_bearers(du_ue_index_t ue_index, span<const lcid_t> lcids);

  /// \brief Returns UE Contention Resolution Id, which is derived from Msg3 bytes.
  ue_con_res_id_t get_con_res_id(rnti_t rnti);

  /// Check if UE with provided C-RNTI exists.
  /// \param rnti C-RNTI of the UE.
  /// \return True if UE exists. False, otherwise.
  bool contains_rnti(rnti_t rnti) const
  {
    const du_ue_index_t ue_index = rnti_table[rnti];
    if (is_du_ue_index_valid(ue_index)) {
      return ue_db.contains(ue_index);
    }
    return false;
  }

  /// Checks if Logical Channel with provided C-RNTI and LCID exists.
  bool contains_lcid(rnti_t rnti, lcid_t lcid) const
  {
    const du_ue_index_t ue_index = rnti_table[rnti];
    if (is_du_ue_index_valid(ue_index)) {
      return ue_db.contains(ue_index) and ue_db[ue_index].logical_channels().contains(lcid);
    }
    return false;
  }

private:
  du_rnti_table& rnti_table;

  // List of added UEs.
  du_ue_list<mac_dl_ue_context> ue_db;
};

} // namespace srsran
