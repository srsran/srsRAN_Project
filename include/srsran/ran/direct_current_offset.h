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

#include "resource_block.h"
#include "srsran/adt/strong_type.h"
#include "srsran/ran/resource_allocation/rb_interval.h"
#include "srsran/support/srsran_assert.h"

namespace srsran {

/// \brief Direct Current (DC) offset, in number of subcarriers. See "txDirectCurrentLocation" in TS 38.331.
///
/// The numerology of the active UL/DL BWP is used as a reference to determine the number of subcarriers of the DC
/// offset.
/// The DC offset value 0 corresponds to the center of the SCS-Carrier for the numerology of the active UL/DL BWP. The
/// relation with the TS 38.331 "txDirectCurrentLocation" parameter is, therefore, given by: dc_offset =
/// txDirectCurrentLocation - 12 * N_RB / 2, where "N_RB" is the number of RBs of the SCS-Carrier.
/// In case the DC offset falls within the SCS-Carrier boundaries, its value should range between {0,...,12 * N_RB - 1}.
enum class dc_offset_t : int {
  min          = -static_cast<int>(NOF_SUBCARRIERS_PER_RB * MAX_NOF_PRBS) / 2,
  center       = 0,
  max          = static_cast<int>(NOF_SUBCARRIERS_PER_RB * MAX_NOF_PRBS) / 2 - 1,
  outside      = static_cast<int>(NOF_SUBCARRIERS_PER_RB * MAX_NOF_PRBS) / 2,
  undetermined = std::numeric_limits<int>::max()
};

namespace dc_offset_helper {

/// \brief Pack the DC offset into a 16-bit unsigned integer used in the RRC and FAPI messages. See
/// "txDirectCurrentLocation" in TS 38.331.
/// \return Packed DC offset value. In case the DC offset falls within the SCS-Carrier boundaries, its value should
/// range between {0,...,12 * carrier_nof_rbs - 1}. In case the DC offset falls outside the SCS-Carrier, its value
/// is set to 3300. In case of undetermined DC offset, its value is set to 3301.
inline uint16_t pack(dc_offset_t offset, unsigned carrier_nof_rbs)
{
  if (offset == dc_offset_t::undetermined) {
    return 3301;
  }

  // Case where DC offset inside spectrum.
  const int nof_subcarriers = static_cast<int>(NOF_SUBCARRIERS_PER_RB * carrier_nof_rbs);
  auto      value           = static_cast<std::underlying_type_t<dc_offset_t>>(offset);
  if (value >= -nof_subcarriers / 2 && value < nof_subcarriers / 2) {
    return value + nof_subcarriers / 2;
  }

  // Case DC offset falls outside the spectrum.
  return 3300;
}

/// \brief Checks if the DC position is contained in a CRB interval.
/// \param[in] offset          DC offset parameter.
/// \param[in] carrier_nof_rbs Carrier bandwidth in resource blocks.
/// \param[in] rbs             Resource block allocation.
/// \return \c true if the DC position is contained within the allocated CRBs. Otherwise \c false.
inline bool is_contained(dc_offset_t offset, unsigned carrier_nof_rbs, crb_interval rbs)
{
  if (offset == dc_offset_t::undetermined) {
    return false;
  }

  // Case where DC offset inside spectrum.
  const int nof_subcarriers = static_cast<int>(NOF_SUBCARRIERS_PER_RB * carrier_nof_rbs);
  auto      dc_position     = static_cast<std::underlying_type_t<dc_offset_t>>(offset);
  if (dc_position >= -nof_subcarriers / 2 && dc_position < nof_subcarriers / 2) {
    dc_position += nof_subcarriers / 2;
  }

  return rbs.contains(dc_position / NOF_SUBCARRIERS_PER_RB);
}

/// \brief Checks if the DC position is contained in a CRB interval.
/// \param[in] dc_position     DC position in subcarriers within the grid relative to PointA.
/// \param[in] rbs             Resource block allocation.
/// \return \c true if the DC position is contained within the allocated CRBs. Otherwise \c false.
inline bool is_contained(unsigned dc_position, crb_interval rbs)
{
  return rbs.contains(dc_position / NOF_SUBCARRIERS_PER_RB);
}

} // namespace dc_offset_helper

} // namespace srsran
