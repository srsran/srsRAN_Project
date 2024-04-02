/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
class dl_sch_logical_channel_mapper
{
public:
  dl_sch_logical_channel_mapper(du_rnti_table& rnti_table_);

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

  bool remove_ue(du_ue_index_t ue_index);

  bool addmod_bearers(du_ue_index_t ue_index, span<const mac_logical_channel_config> dl_logical_channels);

  bool remove_bearers(du_ue_index_t ue_index, span<const lcid_t> lcids);

  /// \brief Returns UE Contention Resolution Id, which is derived from Msg3 bytes.
  ue_con_res_id_t get_con_res_id(rnti_t rnti);

private:
  du_rnti_table& rnti_table;

  du_ue_list<mac_dl_ue_context> ue_db;
};

} // namespace srsran
