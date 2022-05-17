/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_MAC_UL_UE_MANAGER_H
#define SRSGNB_MAC_UL_UE_MANAGER_H

#include "../../ran/gnb_format.h"
#include "../du_rnti_table.h"
#include "../mac_config.h"
#include "mac_ul_sch_pdu.h"
#include "srsgnb/adt/slot_array.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/du_ue_list.h"

namespace srsgnb {

/// Stores MAC UL UE context. In particular, the UL PDU notifiers for each logical channel.
class mac_ul_ue_context
{
public:
  explicit mac_ul_ue_context(du_ue_index_t ue_index_, rnti_t rnti_) : ue_index(ue_index_), rnti(rnti_) {}

  const du_ue_index_t ue_index = MAX_NOF_DU_UES;
  const rnti_t        rnti     = INVALID_RNTI;

  /// List of UL PDU notification endpoints associated to UE's logical channels.
  slot_vector<mac_sdu_rx_notifier*> ul_bearers;
};

/// Class that manages the creation/reconfiguration/deletion of UEs from the MAC UL
/// Note: The access to this class is thread-safe as long as the same UE index and RNTI % MAX_NOF_UES is not accessed
///       from different threads. This is in general assured by the DU manager.
class mac_ul_ue_manager
{
public:
  mac_ul_ue_manager(mac_common_config_t& cfg_, du_rnti_table& rnti_table_) :
    cfg(cfg_), logger(cfg.logger), rnti_table(rnti_table_)
  {}

  /// Checks whether RNTI exists.
  bool contains_rnti(rnti_t rnti) const { return ue_db.contains(rnti_table[rnti]); }

  /// Adds UE in MAC UL UE repository
  bool add_ue(const mac_ue_create_request_message& request)
  {
    srsran_sanity_check(is_crnti(request.crnti), "Invalid C-RNTI={:#x}", request.crnti);

    // 1. Insert UE
    if (ue_db.contains(request.ue_index)) {
      return false;
    }
    ue_db.emplace(request.ue_index, request.ue_index, request.crnti);

    // 2. Add UE Bearers
    if (not addmod_bearers(request.ue_index, request.bearers)) {
      log_proc_failure(logger, request.ue_index, request.crnti, "UE Create Request", "Failed to add/mod UE bearers");
      return false;
    }

    return true;
  }

  bool reconfigure_ue(const mac_ue_reconfiguration_request_message& request)
  {
    // 1. Remove UE bearers
    if (not remove_bearers(request.ue_index, request.bearers_to_rem)) {
      log_proc_failure(logger,
                       request.ue_index,
                       request.crnti,
                       "UE Reconfiguration Request",
                       "Failed to update UL bearers in DEMUX");
      return false;
    }

    // 2. Add/Mod UE Bearers
    if (not addmod_bearers(request.ue_index, request.bearers_to_addmod)) {
      log_proc_failure(logger,
                       request.ue_index,
                       request.crnti,
                       "UE Reconfiguration Request",
                       "Failed to update UL bearers in DEMUX");
      return false;
    }
    return true;
  }

  void remove_ue(du_ue_index_t ue_index)
  {
    if (not ue_db.contains(ue_index)) {
      log_proc_failure(logger, ue_index, "UE Remove Request", "Invalid RNTI");
      return;
    }
    ue_db.erase(ue_index);
  }

  mac_ul_ue_context* find_ue(du_ue_index_t ue_index)
  {
    if (ue_db.contains(ue_index)) {
      return &ue_db[ue_index];
    }
    return nullptr;
  }

private:
  bool addmod_bearers(du_ue_index_t ue_index, const std::vector<mac_logical_channel_addmod>& ul_logical_channels)
  {
    if (not ue_db.contains(ue_index)) {
      return false;
    }
    mac_ul_ue_context& u = ue_db[ue_index];

    for (const mac_logical_channel_addmod& channel : ul_logical_channels) {
      u.ul_bearers.insert(channel.lcid, channel.ul_bearer);
    }

    return true;
  }

  bool remove_bearers(du_ue_index_t ue_index, span<const lcid_t> lcids)
  {
    if (not ue_db.contains(ue_index)) {
      return false;
    }
    mac_ul_ue_context& u = ue_db[ue_index];

    for (lcid_t lcid : lcids) {
      u.ul_bearers.erase(lcid);
    }
    return true;
  }

  /// Arguments of UE manager.
  mac_common_config_t&  cfg;
  srslog::basic_logger& logger;
  du_rnti_table&        rnti_table;

  /// MAC UL UE repository.
  du_ue_list<mac_ul_ue_context> ue_db;
};

} // namespace srsgnb

#endif // SRSGNB_MAC_UL_UE_MANAGER_H
