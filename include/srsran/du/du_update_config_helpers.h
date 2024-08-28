/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/expected.h"
#include "srsran/du/du_cell_config.h"

namespace srsran {
namespace config_helpers {

/// \brief Compute the PRACH frequency start as a function of the PUCCH guardbands.
///
/// This function computes the PRACH frequency start so as it won't collide with the PUCCH resources. As per TS 38.331,
/// PRACH frequency start is \c msg1-FrequencyStart.
///
/// \param user_params parameters passed by the user for the generation the PUCCH resource list.
/// \param bwp_size size of the BWP in RBs.
/// \return PRACH frequency start.
unsigned compute_prach_frequency_start(const pucch_builder_params& user_params, unsigned bwp_size, bool is_long_prach);

/// \brief Compute the number of PUCCH resources that are used for SR and CSI.
///
/// If the user has defined a larger number of resources for SR and CSI than needed, this will returns the actual
/// estimated number of resources that will be used by the GNB, constrained by the maximum PUCCH grants per slot.
/// \param user_params parameters passed by the user for the generation the PUCCH resource list.
/// \param max_pucch_grants_per_slot maximum number of PUCCH grants that can be allocated per slot in the cell.
/// \param sr_period_msec SR period in milliseconds.
/// \param csi_period_msec CSI period in milliseconds.
void compute_nof_sr_csi_pucch_res(pucch_builder_params&   user_params,
                                  unsigned                max_pucch_grants_per_slot,
                                  float                   sr_period_msec,
                                  std::optional<unsigned> csi_period_msec);

bounded_integer<unsigned, 1, 14> compute_max_nof_pucch_symbols(const srs_builder_params& user_srs_params);

} // namespace config_helpers
} // namespace srsran
