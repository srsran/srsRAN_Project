
#ifndef SRSGNB_MAC_UL_UE_MANAGER_H
#define SRSGNB_MAC_UL_UE_MANAGER_H

#include "../../ran/gnb_format.h"
#include "../mac_config.h"
#include "mac_ul_sch_pdu.h"
#include "srsgnb/adt/circular_map.h"
#include "srsgnb/adt/slot_array.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/rnti_map.h"

namespace srsgnb {

/// Stores MAC UL UE context. In particular, the UL PDU notifiers for each logical channel.
class mac_ul_ue_context
{
public:
  explicit mac_ul_ue_context(rnti_t rnti_, du_ue_index_t ue_index_) : rnti(rnti_), ue_index(ue_index_) {}

  const rnti_t        rnti     = INVALID_RNTI;
  const du_ue_index_t ue_index = MAX_NOF_UES;

  /// List of UL PDU notification endpoints associated to UE's logical channels.
  slot_vector<mac_sdu_rx_notifier*> ul_bearers;
};

/// Class that manages the creation/reconfiguration/deletion of UEs from the MAC UL
/// Note: The access to this class is thread-safe as long as the same UE index and RNTI % MAX_NOF_UES is not accessed
///       from different threads. This is in general assured by the DU manager.
class mac_ul_ue_manager
{
public:
  mac_ul_ue_manager(mac_common_config_t& cfg_) : cfg(cfg_), logger(cfg.logger) {}

  /// Checks whether RNTI exists.
  bool contains_ue(rnti_t rnti) const { return ue_db.contains(rnti); }

  /// Adds UE in MAC UL UE repository
  bool add_ue(const mac_ue_create_request_message& request)
  {
    srsran_assert(ue_db.has_space(request.crnti), "No space for RNTI={:#x}", request.crnti);

    // 1. Insert UE
    ue_db.insert(request.crnti, mac_ul_ue_context{request.crnti, request.ue_index});

    // 2. Add UE Bearers
    if (not addmod_bearers(request.crnti, request.bearers)) {
      log_proc_failure(logger, request.ue_index, request.crnti, "UE Create Request", "Failed to add/mod UE bearers");
      return false;
    }

    return true;
  }

  bool reconfigure_ue(const mac_ue_reconfiguration_request_message& request)
  {
    // 1. Remove UE bearers
    if (not remove_bearers(request.crnti, request.bearers_to_rem)) {
      log_proc_failure(logger,
                       request.ue_index,
                       request.crnti,
                       "UE Reconfiguration Request",
                       "Failed to update UL bearers in DEMUX");
      return false;
    }

    // 2. Add/Mod UE Bearers
    if (not addmod_bearers(request.crnti, request.bearers_to_addmod)) {
      log_proc_failure(logger,
                       request.ue_index,
                       request.crnti,
                       "UE Reconfiguration Request",
                       "Failed to update UL bearers in DEMUX");
      return false;
    }
    return true;
  }

  void remove_ue(rnti_t rnti)
  {
    if (not ue_db.erase(rnti)) {
      log_proc_failure(logger, rnti, "UE Remove Request", "Invalid RNTI");
    }
  }

  mac_ul_ue_context* find_rnti(rnti_t rnti)
  {
    auto it = ue_db.find(rnti);
    return it == ue_db.end() ? nullptr : &it->second;
  }

private:
  bool addmod_bearers(rnti_t rnti, const std::vector<logical_channel_addmod>& ul_logical_channels)
  {
    mac_ul_ue_context* ue = find_rnti(rnti);
    if (ue == nullptr) {
      return false;
    }

    for (const logical_channel_addmod& channel : ul_logical_channels) {
      ue->ul_bearers.insert(channel.lcid, channel.ul_bearer);
    }

    return true;
  }

  bool remove_bearers(rnti_t rnti, span<const lcid_t> lcids)
  {
    mac_ul_ue_context* ue = find_rnti(rnti);
    if (ue == nullptr) {
      return false;
    }
    for (lcid_t lcid : lcids) {
      ue->ul_bearers.erase(lcid);
    }
    return true;
  }

  /// Arguments of UE manager.
  mac_common_config_t&  cfg;
  srslog::basic_logger& logger;

  /// MAC UL UE repository.
  rnti_map<mac_ul_ue_context> ue_db;
};

} // namespace srsgnb

#endif // SRSGNB_MAC_UL_UE_MANAGER_H
