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

#pragma once

#include "config_ptr.h"
#include "srsran/scheduler/config/logical_channel_config.h"

namespace srsran {

/// Pointer to managed logical channel configuration.
using logical_channel_config_ptr = config_ptr<logical_channel_config>;

/// \brief List of configured logical channels for a given UE.
///
/// This class provides several lookup mechanisms to facilitate scheduling.
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
    srsran_sanity_check(std::is_sorted(lc_list.begin(),
                                       lc_list.end(),
                                       [](const auto& lhs, const auto& rhs) { return lhs->lcid < rhs->lcid; }),
                        "Configured logical channels must be sorted by LCID.");

    // Build lookup LCID -> LC configuration.
    lcid_t max_lcid = lc_list.back()->lcid;
    lcid_to_lc_cfg_lookup.resize(max_lcid + 1, INVALID_LCID);
    for (unsigned i = 0, e = lc_list.size(); i != e; ++i) {
      lcid_to_lc_cfg_lookup[lc_list[i]->lcid] = i;
    }

    // Build list of LCG-IDs.
    auto max_lcg_it = std::max_element(
        lc_list.begin(), lc_list.end(), [](const auto& lhs, const auto& rhs) { return lhs->lc_group < rhs->lc_group; });
    lcg_id_map.resize((*max_lcg_it)->lc_group + 1, false);
    for (unsigned i = 0, e = lc_list.size(); i != e; ++i) {
      lcg_id_map[lc_list[i]->lc_group] = true;
    }
  }

  /// List of logical channels ordered by LCID.
  span<const config_ptr<logical_channel_config>> logical_channels() const { return lc_list; }

  /// Checks whether the LCID is present in the list.
  bool contains(lcid_t lcid) const
  {
    return lcid < lcid_to_lc_cfg_lookup.size() and lcid_to_lc_cfg_lookup[lcid] != INVALID_LCID;
  }

  /// Retrieve logical channel configuration for a given LCID.
  logical_channel_config_ptr operator[](lcid_t lcid) const { return lc_list[lcid_to_lc_cfg_lookup[lcid]]; }

  /// Checks whether the LCG-ID is present in the list.
  bool contains(lcg_id_t lcg_id) const { return lcg_id < lcg_id_map.size() and lcg_id_map[lcg_id]; }

  /// Number of configured logical channels.
  size_t size() const { return lc_list.size(); }

  /// Comparison of logical channel config lists.
  bool operator==(const logical_channel_config_list& rhs) const { return lc_list == rhs.lc_list; }
  bool operator!=(const logical_channel_config_list& rhs) const { return not(*this == rhs); }

  /// Iterate through list of logical channels.
  iterator       begin() { return lc_list.begin(); }
  const_iterator begin() const { return lc_list.begin(); }
  iterator       end() { return lc_list.end(); }
  const_iterator end() const { return lc_list.end(); }

private:
  // List of logical channel configurations.
  std::vector<config_ptr<logical_channel_config>> lc_list;

  // Lookup of LCID to LC config.
  std::vector<unsigned> lcid_to_lc_cfg_lookup;

  // Lookup of LCG-ID to list of LC configs.
  std::vector<bool> lcg_id_map;
};

/// Pointer to managed list of logical channel configurations.
using logical_channel_config_list_ptr = config_ptr<logical_channel_config_list>;

} // namespace srsran
