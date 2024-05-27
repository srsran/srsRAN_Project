/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_ue_bearer_manager.h"

using namespace srsran;
using namespace srs_du;

du_ue_srb&
du_ue_bearer_manager::add_srb(srb_id_t srb_id, const rlc_config& rlc_cfg, std::optional<const mac_lc_config> mac_cfg)
{
  srsran_assert(not srbs().contains(srb_id), "SRB-Id={} already exists", srb_id);
  srbs_.emplace(srb_id);
  srbs_[srb_id].srb_id  = srb_id;
  srbs_[srb_id].rlc_cfg = rlc_cfg;
  if (mac_cfg.has_value()) {
    srbs_[srb_id].mac_cfg = *mac_cfg;
  }
  return srbs_[srb_id];
}

void du_ue_bearer_manager::add_drb(std::unique_ptr<du_ue_drb> drb)
{
  srsran_assert(drbs().count(drb->drb_id) == 0, "DRB-Id={} already exists", drb->drb_id);
  drbs_.emplace(drb->drb_id, std::move(drb));
}

std::unique_ptr<du_ue_drb> du_ue_bearer_manager::remove_drb(drb_id_t drb_id)
{
  srsran_assert(drbs().count(drb_id) > 0, "DRB-Id={} does not exist", drb_id);
  std::unique_ptr<du_ue_drb> drb = std::move(drbs_.at(drb_id));
  drbs_.erase(drb_id);
  return drb;
}

std::optional<lcid_t> du_ue_bearer_manager::allocate_lcid() const
{
  static_vector<lcid_t, MAX_NOF_DRBS> used_lcids;
  for (const auto& drb : drbs()) {
    used_lcids.push_back(drb.second->lcid);
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
    return {};
  }
  return lcid;
}
