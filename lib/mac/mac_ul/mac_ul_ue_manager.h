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

#include "../../ran/gnb_format.h"
#include "srsran/adt/slotted_array.h"
#include "srsran/du_high/rnti_value_table.h"
#include "srsran/mac/mac.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/du_ue_list.h"

namespace srsran {

using du_rnti_table = rnti_value_table<du_ue_index_t, du_ue_index_t::INVALID_DU_UE_INDEX>;

/// Stores MAC UL UE context. In particular, the UL PDU notifiers for each logical channel.
class mac_ul_ue_context
{
public:
  explicit mac_ul_ue_context(du_ue_index_t ue_index_, rnti_t rnti_) : ue_index(ue_index_), rnti(rnti_) {}

  const du_ue_index_t ue_index = MAX_NOF_DU_UES;
  const rnti_t        rnti     = INVALID_RNTI;

  /// List of UL PDU notification endpoints associated to UE's logical channels.
  slotted_vector<mac_sdu_rx_notifier*> ul_bearers;
};

/// Class that manages the creation/reconfiguration/deletion of UEs from the MAC UL
/// Note: The access to this class is thread-safe as long as the same UE index and RNTI % MAX_NOF_UES is not accessed
///       from different threads. This is in general assured by the DU manager.
class mac_ul_ue_manager
{
public:
  mac_ul_ue_manager(du_rnti_table& rnti_table_);

  /// Checks whether RNTI exists.
  bool contains_rnti(rnti_t rnti) const { return ue_db.contains(rnti_table[rnti]); }

  /// Adds UE in MAC UL UE repository
  bool add_ue(const mac_ue_create_request& request);

  /// Add bearers to existing UE.
  bool addmod_bearers(du_ue_index_t ue_index, const std::vector<mac_logical_channel_config>& ul_logical_channels);

  /// Remove bearers based on LCID.
  bool remove_bearers(du_ue_index_t ue_index, span<const lcid_t> lcids);

  void remove_ue(du_ue_index_t ue_index);

  mac_ul_ue_context* find_ue(du_ue_index_t ue_index)
  {
    if (ue_db.contains(ue_index)) {
      return &ue_db[ue_index];
    }
    return nullptr;
  }

private:
  /// Arguments of UE manager.
  srslog::basic_logger& logger;
  du_rnti_table&        rnti_table;

  /// MAC UL UE repository.
  du_ue_list<mac_ul_ue_context> ue_db;
};

} // namespace srsran
