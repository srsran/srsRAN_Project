
#ifndef SRSGNB_MAC_UL_UE_MANAGER_H
#define SRSGNB_MAC_UL_UE_MANAGER_H

#include "../../ran/gnb_format.h"
#include "../mac_config.h"
#include "mac_ul_sch_pdu.h"
#include "srsgnb/adt/circular_map.h"
#include "srsgnb/adt/slot_array.h"
#include "srsgnb/mac/mac.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/rnti.h"

namespace srsgnb {

/// Stores MAC UL UE context
class mac_ul_ue
{
public:
  explicit mac_ul_ue(du_ue_index_t ue_index_, rnti_t rnti_, mac_rx_sdu_notifier& ccch_notifier_) :
    ue_index(ue_index_), rnti(rnti_)
  {
    ul_bearers.insert(0, &ccch_notifier_);
  }

  const du_ue_index_t               ue_index = MAX_NOF_UES;
  const rnti_t                      rnti     = INVALID_RNTI;
  slot_vector<mac_rx_sdu_notifier*> ul_bearers;
  mac_ul_sch_pdu                    pdu_ul;
};

/// Class that manages the creation/reconfiguration/deletion of UEs from the MAC UL
/// Note: The access to this class is thread-safe as long as the same UE index and RNTI % MAX_NOF_UES is not accessed
///       from different threads. This is in general assured by the DU manager.
class mac_ul_ue_manager
{
public:
  mac_ul_ue_manager(mac_common_config_t& cfg_, mac_rx_sdu_notifier& ccch_notifier_) :
    cfg(cfg_), logger(cfg.logger), ccch_notifier(ccch_notifier_)
  {
    std::fill(ue_index_to_rnti.begin(), ue_index_to_rnti.end(), MAX_NOF_UES);
  }

  /// Checks whether UE Index exists.
  bool contains_ue(du_ue_index_t ue_index) const { return ue_index_to_rnti[ue_index] < MAX_NOF_UES; }

  /// Checks whether RNTI exists.
  bool contains_rnti(rnti_t rnti) const { return ue_db.contains(rnti); }

  /// Adds UE in MAC UL UE repository
  bool add_ue(const mac_ue_create_request_message& request)
  {
    srsran_sanity_check(request.ue_index < MAX_NOF_UES, "Invalid ueId={}", request.ue_index);

    // Verify if there is space.
    if (contains_ue(request.ue_index) or not ue_db.has_space(request.crnti)) {
      log_proc_failure(logger, request.ue_index, request.crnti, "UE Create Request", "Failed to create UE");
      return false;
    }

    // 1. Insert UE
    ue_db.insert(request.crnti, mac_ul_ue{request.ue_index, request.crnti, ccch_notifier});
    ue_index_to_rnti[request.ue_index] = request.crnti;

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
    srsran_sanity_check(ue_index < MAX_NOF_UES, "Invalid ueId={}", ue_index);
    rnti_t rnti = ue_index_to_rnti[ue_index];
    if (not ue_db.erase(rnti)) {
      return;
    }
    ue_index_to_rnti[ue_index] = MAX_NOF_UES;
  }

  mac_ul_ue* find_ue(du_ue_index_t ue_index)
  {
    srsran_sanity_check(ue_index < MAX_NOF_UES, "Invalid ueId={}", ue_index);
    rnti_t rnti = ue_index_to_rnti[ue_index];
    return find_rnti(rnti);
  }

  mac_ul_ue* find_rnti(rnti_t rnti)
  {
    auto it = ue_db.find(rnti);
    return it == ue_db.end() ? nullptr : &it->second;
  }

private:
  bool addmod_bearers(du_ue_index_t ue_index, const std::vector<logical_channel_addmod>& ul_logical_channels)
  {
    mac_ul_ue* ue = find_ue(ue_index);
    if (ue == nullptr) {
      return false;
    }

    for (const logical_channel_addmod& channel : ul_logical_channels) {
      ue->ul_bearers.insert(channel.lcid, channel.ul_bearer);
    }

    return true;
  }

  bool remove_bearers(du_ue_index_t ue_index, span<const lcid_t> lcids)
  {
    mac_ul_ue* ue = find_ue(ue_index);
    if (ue == nullptr) {
      return false;
    }
    for (lcid_t lcid : lcids) {
      ue->ul_bearers.erase(lcid);
    }
    return true;
  }

  /// Arguments of UE manager
  mac_common_config_t&  cfg;
  srslog::basic_logger& logger;
  mac_rx_sdu_notifier&  ccch_notifier;

  /// UE repository
  circular_map<rnti_t, mac_ul_ue, MAX_NOF_UES> ue_db;
  std::array<du_ue_index_t, MAX_NOF_UES>       ue_index_to_rnti;
};

} // namespace srsgnb

#endif // SRSGNB_MAC_UL_UE_MANAGER_H
