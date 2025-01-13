/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "du_bearer_resource_manager.h"
#include "srsran/mac/config/mac_config_helpers.h"
#include "srsran/rlc/rlc_srb_config_factory.h"

using namespace srsran;
using namespace srs_du;

/// \brief Finds an unused LCID for DRBs given a list of UE configured RLC bearers.
static lcid_t find_empty_lcid(const slotted_id_vector<drb_id_t, du_ue_drb_config>& drbs)
{
  static_vector<lcid_t, MAX_NOF_DRBS> used_lcids;
  for (const auto& drb : drbs) {
    used_lcids.push_back(drb.lcid);
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

static error_type<std::string> validate_drb_setup_request(const f1ap_drb_to_setup&                             drb,
                                                          const slotted_id_vector<drb_id_t, du_ue_drb_config>& drb_list,
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
  if (drb_list.contains(drb.drb_id)) {
    return make_unexpected("DRB-Id already exists");
  }

  return {};
}

static error_type<std::string>
validate_drb_modification_request(const f1ap_drb_to_modify&                            drb,
                                  const slotted_id_vector<drb_id_t, du_ue_drb_config>& drb_list)
{
  // Search for established DRB with matching DRB-Id.
  if (not drb_list.contains(drb.drb_id)) {
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
  for (const du_ue_srb_config& old_srb : old_ue_cfg.srbs) {
    new_ue_cfg.srbs.emplace(old_srb.srb_id, old_srb);
  }
  for (const du_ue_drb_config& old_drb : old_ue_cfg.drbs) {
    new_ue_cfg.drbs.emplace(old_drb.drb_id, old_drb);
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

  return resp;
}

void du_bearer_resource_manager::setup_srbs(du_ue_resource_config&                      ue_cfg,
                                            const du_ue_bearer_resource_update_request& upd_req)
{
  for (srb_id_t srb_id : upd_req.srbs_to_setup) {
    if (ue_cfg.srbs.contains(srb_id)) {
      // The SRB is already setup (e.g. SRB1 gets setup automatically).
      continue;
    }

    ue_cfg.srbs.emplace(srb_id);
    du_ue_srb_config& new_srb = ue_cfg.srbs[srb_id];
    new_srb.srb_id            = srb_id;
    auto srb_config_it        = srb_config.find(srb_id);
    if (srb_config_it != srb_config.end()) {
      new_srb.rlc_cfg = srb_config_it->second.rlc;
    } else {
      new_srb.rlc_cfg = make_default_srb_rlc_config();
    }
    new_srb.mac_cfg = make_default_srb_mac_lc_config(srb_id_to_lcid(srb_id));
  }
}

std::vector<drb_id_t> du_bearer_resource_manager::setup_drbs(du_ue_resource_config&                      ue_cfg,
                                                             const du_ue_bearer_resource_update_request& upd_req)
{
  std::vector<drb_id_t> failed_drbs;

  for (const f1ap_drb_to_setup& drb_to_setup : upd_req.drbs_to_setup) {
    auto res = validate_drb_setup_request(drb_to_setup, ue_cfg.drbs, qos_config);
    if (not res.has_value()) {
      failed_drbs.push_back(drb_to_setup.drb_id);
      logger.warning("Failed to allocate {}. Cause: {}", drb_to_setup.drb_id, res.error());
      continue;
    }

    // Allocate LCID.
    lcid_t lcid = find_empty_lcid(ue_cfg.drbs);
    if (lcid > LCID_MAX_DRB) {
      logger.warning("Failed to allocate {}. Cause: No available LCIDs", drb_to_setup.drb_id);
      failed_drbs.push_back(drb_to_setup.drb_id);
      continue;
    }

    // Get QoS Config from 5QI
    five_qi_t            fiveqi = drb_to_setup.qos_info.drb_qos.qos_desc.get_5qi();
    const du_qos_config& qos    = qos_config.at(fiveqi);

    // Create new DRB QoS Flow.
    du_ue_drb_config& new_drb = ue_cfg.drbs.emplace(drb_to_setup.drb_id);
    new_drb.drb_id            = drb_to_setup.drb_id;
    new_drb.lcid              = lcid;
    new_drb.pdcp_sn_len       = drb_to_setup.pdcp_sn_len;
    new_drb.s_nssai           = drb_to_setup.qos_info.s_nssai;
    new_drb.qos               = drb_to_setup.qos_info.drb_qos;
    new_drb.f1u               = qos.f1u;
    new_drb.rlc_cfg           = qos.rlc;
    new_drb.mac_cfg           = make_non_gbr_drb_mac_lc_config();
    if (drb_to_setup.qos_info.drb_qos.gbr_qos_info.has_value()) {
      // Populate MAC LC configuration for GBR DRB if GBR QoS information is present.
      new_drb.mac_cfg = make_gbr_drb_mac_lc_config(*drb_to_setup.qos_info.drb_qos.gbr_qos_info);
    }

    // Update pdcp_sn_len in RLC config
    auto& rlc_cfg = new_drb.rlc_cfg;
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
    auto res = validate_drb_modification_request(drb_to_modify, ue_cfg.drbs);
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
    if (not ue_cfg.drbs.contains(drb_id)) {
      logger.warning("Failed to release {}. Cause: DRB not found", drb_id);
      continue;
    }

    // Remove DRB
    ue_cfg.drbs.erase(drb_id);
  }
}
