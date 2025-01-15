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

#include "srsran/ran/pdcch/search_space.h"
#include "srsran/scheduler/config/bwp_configuration.h"
#include "srsran/scheduler/result/dci_info.h"

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
