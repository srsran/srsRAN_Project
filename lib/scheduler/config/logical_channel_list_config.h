/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "config_ptr.h"
#include "srsran/scheduler/config/logical_channel_config.h"

namespace srsran {

using logical_channel_config_ptr = config_ptr<logical_channel_config>;

class logical_channel_config_list
{
public:
  using value_type     = config_ptr<logical_channel_config>;
  using iterator       = std::vector<config_ptr<logical_channel_config>>::iterator;
  using const_iterator = std::vector<config_ptr<logical_channel_config>>::const_iterator;

  logical_channel_config_list(std::vector<config_ptr<logical_channel_config>> lc_list_) : lc_list(std::move(lc_list_))
  {
    if (lc_list.empty()) {
      return;
    }
    auto max_it = std::max_element(
        lc_list.begin(), lc_list.end(), [](const auto& lhs, const auto& rhs) { return lhs->lcid < rhs->lcid; });
    lcid_to_lc_cfg_lookup.resize((*max_it)->lcid + 1, lc_list.size());
    for (unsigned i = 0, e = lc_list.size(); i != e; ++i) {
      lcid_to_lc_cfg_lookup[lc_list[i]->lcid] = i;
    }

    auto max_lcg_it = std::max_element(
        lc_list.begin(), lc_list.end(), [](const auto& lhs, const auto& rhs) { return lhs->lc_group < rhs->lc_group; });
    lcg_id_to_lc_cfg_lookup.resize((*max_lcg_it)->lc_group + 1, lc_list.size());
    for (unsigned i = 0, e = lc_list.size(); i != e; ++i) {
      lcg_id_to_lc_cfg_lookup[lc_list[i]->lc_group] = i;
    }
  }

  /// List of logical channels ordered by LCID.
  span<const config_ptr<logical_channel_config>> logical_channels() const { return lc_list; }

  /// Checks whether the LCID is present in the list.
  bool contains(lcid_t lcid) const
  {
    return lcid < lcid_to_lc_cfg_lookup.size() and lcid_to_lc_cfg_lookup[lcid] != lc_list.size();
  }

  /// Checks whether the LCG-ID is present in the list.
  bool contains(lcg_id_t lcg_id) const
  {
    return lcg_id < lcg_id_to_lc_cfg_lookup.size() and lcid_to_lc_cfg_lookup[lcg_id] != lc_list.size();
  }

  logical_channel_config_ptr operator[](lcid_t lcid) const { return lc_list[lcid_to_lc_cfg_lookup[lcid]]; }
  logical_channel_config_ptr operator[](lcg_id_t lcgid) const { return lc_list[lcg_id_to_lc_cfg_lookup[lcgid]]; }

  size_t size() const { return lc_list.size(); }

  bool operator==(const logical_channel_config_list& rhs) const { return lc_list == rhs.lc_list; }
  bool operator!=(const logical_channel_config_list& rhs) const { return not(*this == rhs); }

  iterator       begin() { return lc_list.begin(); }
  const_iterator begin() const { return lc_list.begin(); }
  iterator       end() { return lc_list.end(); }
  const_iterator end() const { return lc_list.end(); }

private:
  std::vector<config_ptr<logical_channel_config>> lc_list;

  std::vector<unsigned> lcid_to_lc_cfg_lookup;
  std::vector<unsigned> lcg_id_to_lc_cfg_lookup;
};

using logical_channel_config_list_ptr = config_ptr<logical_channel_config_list>;

} // namespace srsran
