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

#include "srsran/adt/static_vector.h"
#include "srsran/ran/pucch/pucch_mapping.h"
#include <cstdint>

/// \file
/// \brief Procedures for selecting PUCCH default resource.
///
/// Helper files for selecting the PUCCH resource before dedicated resources are configured as described in TS38.213
/// Section 9.2.1.
///
namespace srsran {

/// Collects the default PUCCH resource parameters given by TS38.213 Table 9.2.1-1.
struct pucch_default_resource {
  /// PUCCH format.
  pucch_format format;
  /// First symbol index within the slot.
  unsigned first_symbol_index;
  /// Number of symbols.
  unsigned nof_symbols;
  /// PRB offset, parameter \f$RB_{BWP}^{offset}\f$ in TS38.213 Section 9.2.1.
  unsigned rb_bwp_offset;
  /// Set of initial cyclic shift indexes. The number of elements provides the parameter \f$N_{CS}\f$ in TS38.213
  /// Section 9.2.1.
  static_vector<uint8_t, 4> cs_indexes;
};

/// \brief Calculates the default PUCCH resource index \f$r_{PUCCH}\f$.
///
/// It implemented is according to TS38.213 Section 9.2.1.
///
/// \param[in] n_cce   Index of the first CCE for the PDCCH reception, parameter \f$n_{CCE,0}\f$.
/// \param[in] nof_cce Number of CCEs in a CORESET of the PDCCH reception with DCI associated to the PUCCH
/// transmission, Parameter \f$N_{CCE}\f$.
/// \param[in] delta_pri PUCCH resource indicator field in the associated DCI, parameter \f$\Delta _{PRI}\f$.
/// \return PUCCH resource index in with a range of {0, ..., 15}.
unsigned get_pucch_default_resource_index(unsigned n_cce, unsigned nof_cce, unsigned delta_pri);

/// \brief Gets the PRB indexes for PUCCH resource before dedicated PUCCH as per TS38.213 Section 9.2.1.
/// \param[in] r_pucch       PUCCH resource index, parameter \f$r_{PUCCH}\f$.
/// \param[in] rb_bwp_offset PRB offset, parameter \f$RB_{BWP}^{offset}\f$.
/// \param[in] nof_cs        Number of cyclic shifts, parameter \f$N_{CS}\f$.
/// \param[in] N_bwp_size    BWP size, parameter \f$N_{BWP}^{size}\f$.
/// \return A pair of values containing the PRB index for the first and second hops.
std::pair<unsigned, unsigned>
get_pucch_default_prb_index(unsigned r_pucch, unsigned rb_bwp_offset, unsigned nof_cs, unsigned N_bwp_size);

/// \brief Gets the PUCCH cyclic shift for PUCCH resource before dedicated PUCCH as per TS38.213 Section 9.2.1.
/// \param[in] r_pucch PUCCH resource index, parameter \f$r_{PUCCH}\f$.
/// \param[in] nof_cs  Number of cyclic shifts, parameter \f$N_{CS}\f$.
/// \return The PUCCH resource cyclic shift.
unsigned get_pucch_default_cyclic_shift(unsigned r_pucch, unsigned nof_cs);

/// \brief Gets the PUCCH resource parameters before dedicated PUCCH resources are configured.
///
/// The values are given by TS38.213 Table 9.2.1-1.
///
/// \param[in] row_index PUCCH default row index, higher layer parameter \e pucch-ResourceCommon (TS38.331
/// Section 6.3.2, Information Element \e PUCCH-ConfigCommon). Possible values are {0, ... , 15}.
/// \param[in] bwp_size Active BWP size, parameter \f$N_{BWP}^{size}\f$ in TS38.213 Section 9.2.1.
/// \return A valid default PUCCH resource.
/// \remark An assertion is triggered if the row index exceeds its boundaries.
pucch_default_resource get_pucch_default_resource(unsigned row_index, unsigned N_bwp_size);

} // namespace srsran
