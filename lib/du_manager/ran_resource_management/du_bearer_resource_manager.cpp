/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_bearer_resource_manager.h"
#include "srsran/mac/config/mac_config_helpers.h"
#include "srsran/rlc/rlc_srb_config_factory.h"

using namespace srsran;
using namespace srs_du;

template <typename Vec>
static auto find_by_srb_id(srb_id_t srb_id, Vec&& bearers)
{
  return std::find_if(
      bearers.begin(), bearers.end(), [srb_id](const auto& b) { return b.lcid == srb_id_to_lcid(srb_id); });
}

template <typename Vec>
static auto find_by_drb_id(drb_id_t drb_id, Vec&& bearers)
{
  return std::find_if(bearers.begin(), bearers.end(), [drb_id](const auto& b) { return b.drb_id == drb_id; });
}

/// \brief Finds an unused LCID for DRBs given a list of UE configured RLC bearers.
static lcid_t find_empty_lcid(span<const rlc_bearer_config> rlc_bearers)
{
  static_vector<lcid_t, MAX_NOF_DRBS> used_lcids;
  for (const auto& bearer : rlc_bearers) {
    if (bearer.drb_id.has_value()) {
      used_lcids.push_back(bearer.lcid);
    }
  }
  std::sort(used_lcids.begin(), used_lcids.end());
  if (used_lcids.empty() or used_lcids[0] > LCID_MIN_DRB) {
    return LCID_MIN_DRB;
  }
  auto it = std::adjacent_find(used_lcids.begin(), used_lcids.end(), [](lcid_t l, lcid_t r) { return l + 1 < r; });
  if (it == used_lcids.end()) {
    // no gaps found. Use the last value + 1.
    --it;
  }
  // beginning of the gap + 1.
  lcid_t lcid = uint_to_lcid(static_cast<unsigned>(*it) + 1U);
  if (lcid > LCID_MAX_DRB) {
    return INVALID_LCID;
  }
  return lcid;
}

static error_type<std::string> validate_drb_setup_request(const f1ap_drb_to_setup&                  drb,
                                                          span<const rlc_bearer_config>             rlc_bearers,
                                                          const std::map<five_qi_t, du_qos_config>& qos_config)
{
  // Validate QOS config.
  five_qi_t fiveqi = drb.qos_info.drb_qos.qos_desc.get_5qi();
  auto      qos_it = qos_config.find(fiveqi);
  if (qos_it == qos_config.end()) {
    return make_unexpected(fmt::format("No {} 5QI configured", fiveqi));
  }
  const du_qos_config& qos = qos_it->second;
  if (qos.rlc.mode != drb.mode) {
    return make_unexpected(
        fmt::format("RLC mode mismatch for {}. QoS config for {} configures {} but CU-CP requested {}",
                    drb.drb_id,
                    fiveqi,
                    qos.rlc.mode,
                    drb.mode));
  }

  // Validate UL UP TNL INFO.
  if (drb.uluptnl_info_list.empty()) {
    return make_unexpected("No UL UP TNL Info List provided");
  }

  // Search for established DRB with matching DRB-Id.
  auto prev_drb_it = find_by_drb_id(drb.drb_id, rlc_bearers);
  if (prev_drb_it != rlc_bearers.end()) {
    return make_unexpected("DRB-Id already exists");
  }

  return {};
}

static error_type<std::string> validate_drb_modification_request(const f1ap_drb_to_modify&     drb,
                                                                 span<const rlc_bearer_config> rlc_bearers)
{
  // Search for established DRB with matching DRB-Id.
  auto prev_drb_it = find_by_drb_id(drb.drb_id, rlc_bearers);
  if (prev_drb_it == rlc_bearers.end()) {
    return make_unexpected("DRB-Id not found");
  }

  // Validate UL UP TNL INFO.
  if (drb.uluptnl_info_list.empty()) {
    return make_unexpected("No UL UP TNL Info List provided");
  }

  return {};
}

static void reestablish_context(du_ue_resource_config& new_ue_cfg, const du_ue_resource_config& old_ue_cfg)
{
  for (const rlc_bearer_config& old_bearer : old_ue_cfg.cell_group.rlc_bearers) {
    auto it = std::find_if(new_ue_cfg.cell_group.rlc_bearers.begin(),
                           new_ue_cfg.cell_group.rlc_bearers.end(),
                           [&old_bearer](const rlc_bearer_config& item) {
                             return item.drb_id == old_bearer.drb_id and
                                    (item.drb_id.has_value() or (item.lcid == old_bearer.lcid));
                           });
    if (it == new_ue_cfg.cell_group.rlc_bearers.end()) {
      // Bearer not found in new context. Add it.
      new_ue_cfg.cell_group.rlc_bearers.push_back(old_bearer);
      if (old_bearer.drb_id.has_value()) {
        const drb_upper_layer_config& old_drb = *find_by_drb_id(old_bearer.drb_id.value(), old_ue_cfg.drbs);
        new_ue_cfg.drbs.push_back(old_drb);
      }
    } else {
      // Bearer already exists. Overwrite it.
      *it = old_bearer;
      if (old_bearer.drb_id.has_value()) {
        const drb_upper_layer_config& old_drb = *find_by_drb_id(old_bearer.drb_id.value(), old_ue_cfg.drbs);
        drb_upper_layer_config&       new_drb = *find_by_drb_id(old_bearer.drb_id.value(), new_ue_cfg.drbs);
        new_drb                               = old_drb;
      }
    }
  }
}

// du_bearer_resource_manager

du_bearer_resource_manager::du_bearer_resource_manager(const std::map<srb_id_t, du_srb_config>&  srbs_,
                                                       const std::map<five_qi_t, du_qos_config>& qos_,
                                                       srslog::basic_logger&                     logger_) :
  srb_config(srbs_), qos_config(qos_), logger(logger_)
{
}

du_ue_bearer_resource_update_response
du_bearer_resource_manager::update(du_ue_resource_config&                      ue_cfg,
                                   const du_ue_bearer_resource_update_request& upd_req,
                                   const du_ue_resource_config*                reestablished_context)
{
  du_ue_bearer_resource_update_response resp;

  // > In case of RRC Reestablishment, retrieve old DRB context, to be considered in the config update.
  if (reestablished_context != nullptr) {
    reestablish_context(ue_cfg, *reestablished_context);
  }

  // Remove DRBs.
  rem_drbs(ue_cfg, upd_req);

  // Setup SRBs.
  setup_srbs(ue_cfg, upd_req);

  // Setup DRBs.
  resp.drbs_failed_to_setup = setup_drbs(ue_cfg, upd_req);

  // Modify DRBs.
  resp.drbs_failed_to_mod = modify_drbs(ue_cfg, upd_req);

  // Sort RAN bearers by LCID.
  std::sort(ue_cfg.cell_group.rlc_bearers.begin(),
            ue_cfg.cell_group.rlc_bearers.end(),
            [](const auto& lhs, const auto& rhs) { return lhs.lcid < rhs.lcid; });

  // Sort DRBs by DRB ID.
  std::sort(
      ue_cfg.drbs.begin(), ue_cfg.drbs.end(), [](const auto& lhs, const auto& rhs) { return lhs.drb_id < rhs.drb_id; });

  return resp;
}

void du_bearer_resource_manager::setup_srbs(du_ue_resource_config&                      ue_cfg,
                                            const du_ue_bearer_resource_update_request& upd_req)
{
  for (srb_id_t srb_id : upd_req.srbs_to_setup) {
    auto srb_it = find_by_srb_id(srb_id, ue_cfg.cell_group.rlc_bearers);
    if (srb_it != ue_cfg.cell_group.rlc_bearers.end()) {
      // The SRB is already setup (e.g. SRB1 gets setup automatically).
      continue;
    }

    auto   srb_config_it = srb_config.find(srb_id);
    lcid_t lcid          = srb_id_to_lcid(srb_id);
    auto&  new_srb       = ue_cfg.cell_group.rlc_bearers.emplace_back();
    new_srb.lcid         = lcid;
    if (srb_config_it != srb_config.end()) {
      new_srb.rlc_cfg = srb_config_it->second.rlc;
      new_srb.mac_cfg = srb_config_it->second.mac;
    } else {
      new_srb.rlc_cfg = make_default_srb_rlc_config();
      new_srb.mac_cfg = make_default_srb_mac_lc_config(lcid);
    }
  }
}

std::vector<drb_id_t> du_bearer_resource_manager::setup_drbs(du_ue_resource_config&                      ue_cfg,
                                                             const du_ue_bearer_resource_update_request& upd_req)
{
  std::vector<drb_id_t> failed_drbs;

  for (const f1ap_drb_to_setup& drb_to_setup : upd_req.drbs_to_setup) {
    auto res = validate_drb_setup_request(drb_to_setup, ue_cfg.cell_group.rlc_bearers, qos_config);
    if (not res.has_value()) {
      failed_drbs.push_back(drb_to_setup.drb_id);
      logger.warning("Failed to allocate {}. Cause: {}", drb_to_setup.drb_id, res.error());
      continue;
    }

    // Allocate LCID.
    lcid_t lcid = find_empty_lcid(ue_cfg.cell_group.rlc_bearers);
    if (lcid > LCID_MAX_DRB) {
      logger.warning("Failed to allocate {}. Cause: No available LCIDs", drb_to_setup.drb_id);
      failed_drbs.push_back(drb_to_setup.drb_id);
      continue;
    }

    // Get QoS Config from 5QI
    five_qi_t            fiveqi = drb_to_setup.qos_info.drb_qos.qos_desc.get_5qi();
    const du_qos_config& qos    = qos_config.at(fiveqi);

    // Create new DRB QoS Flow.
    drb_upper_layer_config& drb_qos = ue_cfg.drbs.emplace_back();
    drb_qos.drb_id                  = drb_to_setup.drb_id;
    drb_qos.pdcp_sn_len             = drb_to_setup.pdcp_sn_len;
    drb_qos.s_nssai                 = drb_to_setup.qos_info.s_nssai;
    drb_qos.qos                     = drb_to_setup.qos_info.drb_qos;
    drb_qos.f1u                     = qos.f1u;

    // Create new L2 DRB.
    rlc_bearer_config& ran_bearer = ue_cfg.cell_group.rlc_bearers.emplace_back();
    ran_bearer.lcid               = lcid;
    ran_bearer.drb_id             = drb_to_setup.drb_id;
    ran_bearer.rlc_cfg            = qos.rlc;
    ran_bearer.mac_cfg            = qos.mac;

    // Update pdcp_sn_len in RLC config
    auto& rlc_cfg = ran_bearer.rlc_cfg;
    switch (rlc_cfg.mode) {
      case rlc_mode::am:
        rlc_cfg.am.tx.pdcp_sn_len = drb_to_setup.pdcp_sn_len;
        break;
      case rlc_mode::um_bidir:
      case rlc_mode::um_unidir_dl:
        rlc_cfg.um.tx.pdcp_sn_len = drb_to_setup.pdcp_sn_len;
        break;
      default:
        break;
    }
  }

  return failed_drbs;
}

std::vector<drb_id_t> du_bearer_resource_manager::modify_drbs(du_ue_resource_config&                      ue_cfg,
                                                              const du_ue_bearer_resource_update_request& upd_req)
{
  std::vector<drb_id_t> failed_drbs;

  for (const f1ap_drb_to_modify& drb_to_modify : upd_req.drbs_to_mod) {
    auto res = validate_drb_modification_request(drb_to_modify, ue_cfg.cell_group.rlc_bearers);
    if (not res.has_value()) {
      logger.warning("Failed to modify {}. Cause: {}", drb_to_modify.drb_id, res.error());
      failed_drbs.push_back(drb_to_modify.drb_id);
      continue;
    }
  }

  return failed_drbs;
}

void du_bearer_resource_manager::rem_drbs(du_ue_resource_config&                      ue_cfg,
                                          const du_ue_bearer_resource_update_request& upd_req)
{
  for (drb_id_t drb_id : upd_req.drbs_to_rem) {
    auto ran_bearer_it = find_by_drb_id(drb_id, ue_cfg.cell_group.rlc_bearers);
    if (ran_bearer_it == ue_cfg.cell_group.rlc_bearers.end()) {
      logger.warning("Failed to release {}. Cause: DRB not found", drb_id);
      continue;
    }

    auto bearer_qos_it = find_by_drb_id(drb_id, ue_cfg.drbs);

    // Remove DRB
    ue_cfg.cell_group.rlc_bearers.erase(ran_bearer_it);
    ue_cfg.drbs.erase(bearer_qos_it);
  }
}
