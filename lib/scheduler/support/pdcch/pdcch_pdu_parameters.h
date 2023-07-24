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

/// \file
/// \brief PDCCH Modulation and Signalling parameter derivation as per TS38.211, 7.3.2.3 and 7.4.1.3.

#pragma once

#include "srsran/ran/pci.h"
#include "srsran/ran/pdcch/coreset.h"
#include "srsran/ran/pdcch/search_space.h"

namespace srsran {

/// Calculates \f$n_ID\f$ as per TS38.211 7.3.2.3.
/// \return integer within values: {0,1,...,65535}.
inline unsigned
get_scrambling_n_ID(pci_t pci, const coreset_configuration& cs_cfg, const search_space_configuration& ss_cfg)
{
  // For a UE-specific search space [...] equals the higher-layer parameter pdcch-DMRS-ScramblingID if configured,
  if (not ss_cfg.is_common_search_space() and cs_cfg.pdcch_dmrs_scrambling_id.has_value()) {
    return *cs_cfg.pdcch_dmrs_scrambling_id;
  }
  // \f$n_id = N_{ID}^{cell}\f$ otherwise.
  return pci;
}

/// Calculates \f$n_{RNTI}\f$ as per TS38.211, 7.3.2.3.
/// \return integer within values: {0,1,...,65535}.
inline unsigned
get_scrambling_n_RNTI(rnti_t rnti, const coreset_configuration& cs_cfg, const search_space_configuration& ss_cfg)
{
  // \f$n_{RNTI}\f$ is given by the C-RNTI for a PDCCH in a UE-speicfic search space if the higher-layer parameter
  // pdcch-DMRS-ScramblingID is configured.
  if (not ss_cfg.is_common_search_space() and cs_cfg.pdcch_dmrs_scrambling_id.has_value()) {
    return rnti;
  }
  return 0;
}

/// Calculates \f$N_{ID}\f$ as per TS38.211, 7.4.1.3.1.
inline unsigned get_N_ID_dmrs(pci_t pci, const coreset_configuration& cs_cfg)
{
  if (cs_cfg.pdcch_dmrs_scrambling_id.has_value()) {
    return cs_cfg.pdcch_dmrs_scrambling_id.value();
  }
  return pci;
}

} // namespace srsran
