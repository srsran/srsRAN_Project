/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_CONFIG_HELPERS_H
#define SRSGNB_CONFIG_HELPERS_H

#include "srsgnb/scheduler/bwp_configuration.h"

namespace srsgnb {

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

/// Computes the lowest RB used by the CORESET.
inline unsigned get_coreset_start_crb(const coreset_configuration& cs_cfg)
{
  static constexpr unsigned NOF_RBS_PER_GROUP = 6U;
  if (cs_cfg.id == to_coreset_id(0)) {
    return cs_cfg.coreset0_crbs().start();
  }
  uint64_t bits = cs_cfg.freq_domain_resources().to_uint64();
  return find_first_lsb_one(bits) * NOF_RBS_PER_GROUP;
}

/// Computes the CRB interval that delimits CORESET#0.
inline crb_interval get_coreset0_crbs(const pdcch_config_common& pdcch_cfg)
{
  unsigned rb_start = get_coreset_start_crb(*pdcch_cfg.coreset0);
  return {rb_start, rb_start + get_coreset_nof_prbs(*pdcch_cfg.coreset0)};
}

/// Calculates number of CCEs in CORESET.
inline unsigned get_coreset_nof_cces(const coreset_configuration& cs_cfg)
{
  unsigned nof_rbs = get_coreset_nof_prbs(cs_cfg);
  return nof_rbs / pdcch_constants::NOF_REG_PER_CCE;
}

} // namespace srsgnb

#endif // SRSGNB_CONFIG_HELPERS_H
