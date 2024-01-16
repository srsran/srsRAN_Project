/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/ran/pdcch/search_space.h"
#include "srsran/scheduler/config/bwp_configuration.h"
#include "srsran/scheduler/scheduler_dci.h"

namespace srsran {
namespace pusch_helper {

/// \brief Determine CRB limits for PUSCH grant, based on BWP configs, SearchSpace type and DCI format as per
/// TS38.214, 6.1.2.2.2.
///
/// \param dci_type DL DCI format.
/// \param init_ul_bwp Initial UL BWP configuration.
/// \param active_ul_bwp Active UL BWP configuration.
/// \param is_common_ss Flag indicating Common SearchSpace type or not.
/// \return Calculated CRB limits.
inline crb_interval get_ra_crb_limits(dci_ul_rnti_config_type  dci_type,
                                      const bwp_configuration& init_ul_bwp,
                                      const bwp_configuration& active_ul_bwp,
                                      bool                     is_common_ss)
{
  // See TS 38.214, 6.1.2.2.2, Uplink resource allocation type 1.
  if (dci_type == dci_ul_rnti_config_type::c_rnti_f0_0 and is_common_ss) {
    return init_ul_bwp.crbs;
  }
  return active_ul_bwp.crbs;
}

} // namespace pusch_helper
} // namespace srsran
