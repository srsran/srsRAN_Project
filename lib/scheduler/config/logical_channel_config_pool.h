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

#include "config_object_pool.h"
#include "logical_channel_list_config.h"
#include "srsran/scheduler/config/logical_channel_config.h"
#include "srsran/scheduler/config/logical_channel_config_factory.h"

namespace srsran {

class logical_channel_config_pool
{
public:
  logical_channel_config_list_ptr create(span<const logical_channel_config> new_cfg)
  {
    // Obtain list of config_ptrs to logical channels.
    std::vector<config_ptr<logical_channel_config>> lc_chs;
    bool has_srb0 = std::any_of(new_cfg.begin(), new_cfg.end(), [](const auto& e) { return e.lcid == LCID_SRB0; });
    if (has_srb0) {
      lc_chs.reserve(new_cfg.size());
    } else {
      // SRB0 is always configured implicitly.
      // Create an SRB0 logical channel entry if it wasn't included in the config.
      lc_chs.reserve(new_cfg.size() + 1);
      lc_chs.push_back(lc_ch_pool.create(config_helpers::create_default_logical_channel_config(LCID_SRB0)));
    }
    for (const auto& lc : new_cfg) {
      lc_chs.push_back(lc_ch_pool.create(lc));
    }
    std::sort(
        lc_chs.begin(), lc_chs.end(), [](const logical_channel_config_ptr& lhs, const logical_channel_config_ptr& rhs) {
          return lhs->lcid < rhs->lcid;
        });

    logical_channel_config_list lc_ch_list{std::move(lc_chs)};
    return lc_ch_list_pool.create(lc_ch_list);
  }

private:
  config_object_pool<logical_channel_config> lc_ch_pool;

  config_object_pool<logical_channel_config_list> lc_ch_list_pool;
};

} // namespace srsran
