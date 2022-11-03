/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief PDCCH Modulation and Signalling parameter derivation as per TS38.211, 7.3.2.3 and 7.4.1.3.
#pragma once

#include "../../cell/cell_configuration.h"

namespace srsgnb {

/// Calculates \f$n_ID\f$ as per TS38.211 7.3.2.3.
/// \return integer within values: {0,1,...,65535}.
inline unsigned get_scrambling_n_ID(const cell_configuration&         cell_cfg,
                                    const coreset_configuration&      cs_cfg,
                                    const search_space_configuration& ss_cfg)
{
  // For a UE-specific search space [...] equals the higher-layer parameter pdcch-DMRS-ScramblingID if configured,
  if (ss_cfg.type == search_space_configuration::type::ue_dedicated and cs_cfg.pdcch_dmrs_scrambling_id.has_value()) {
    return *cs_cfg.pdcch_dmrs_scrambling_id;
  }
  // \f$n_id = N_{ID}^{cell}\f$ otherwise.
  return cell_cfg.pci;
}

/// Calculates \f$n_{RNTI}\f$ as per TS38.211, 7.3.2.3.
/// \return integer within values: {0,1,...,65535}.
inline unsigned
get_scrambling_n_RNTI(rnti_t rnti, const coreset_configuration& cs_cfg, const search_space_configuration& ss_cfg)
{
  // \f$n_{RNTI}\f$ is given by the C-RNTI for a PDCCH in a UE-speicfic search space if the higher-layer parameter
  // pdcch-DMRS-ScramblingID is configured,
  if (ss_cfg.type == search_space_configuration::type::ue_dedicated and cs_cfg.pdcch_dmrs_scrambling_id.has_value()) {
    return rnti;
  }
  return 0;
}

/// Calculates \f$N_{ID}\f$ as per TS38.211, 7.4.1.3.1.
inline unsigned get_N_ID_dmrs(const cell_configuration& cell_cfg, const coreset_configuration& cs_cfg)
{
  if (cs_cfg.pdcch_dmrs_scrambling_id.has_value()) {
    return cs_cfg.pdcch_dmrs_scrambling_id.value();
  }
  return cell_cfg.pci;
}

} // namespace srsgnb