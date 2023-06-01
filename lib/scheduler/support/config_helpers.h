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

#include "srsran/scheduler/config/bwp_configuration.h"
#include "srsran/scheduler/scheduler_dci.h"

namespace srsran {

/// Retrieves the time resource allocation table for PUSCH.
/// \remark See TS 38.214, Section 6.1.2.1.1 - Determination of the resource allocation table to be used for PUSCH.
inline span<const pusch_time_domain_resource_allocation>
get_pusch_time_domain_resource_table(const pusch_config_common& pusch_cfg)
{
  if (pusch_cfg.pusch_td_alloc_list.empty()) {
    // Use default tables 6.1.2.1.1-2/3.
    // TODO: PHY helper.
  }
  return pusch_cfg.pusch_td_alloc_list;
}

/// Computes the number of RBs used to represent the CORESET.
inline unsigned get_coreset_nof_prbs(const coreset_configuration& cs_cfg)
{
  static constexpr unsigned NOF_RBS_PER_GROUP = 6U;
  if (cs_cfg.id == to_coreset_id(0)) {
    return cs_cfg.coreset0_crbs().length();
  }
  return cs_cfg.freq_domain_resources().count() * NOF_RBS_PER_GROUP;
}

/// Computes the highest RB used by the CORESET.
inline unsigned get_coreset_end_crb(const coreset_configuration& cs_cfg)
{
  static constexpr unsigned NOF_RBS_PER_GROUP = 6U;
  if (cs_cfg.id == to_coreset_id(0)) {
    return cs_cfg.coreset0_crbs().stop();
  }
  const uint64_t highest_bit = cs_cfg.freq_domain_resources().find_highest(0, cs_cfg.freq_domain_resources().size());
  return highest_bit * NOF_RBS_PER_GROUP;
}

/// Computes the CRB interval that delimits CORESET.
inline crb_interval get_coreset_crbs(const coreset_configuration& cs_cfg)
{
  return {cs_cfg.get_coreset_start_crb(), get_coreset_end_crb(cs_cfg)};
}

/// Computes the CRB interval that delimits CORESET#0.
inline crb_interval get_coreset0_crbs(const pdcch_config_common& pdcch_cfg)
{
  unsigned rb_start = pdcch_cfg.coreset0->get_coreset_start_crb();
  return {rb_start, rb_start + get_coreset_nof_prbs(*pdcch_cfg.coreset0)};
}

} // namespace srsran
