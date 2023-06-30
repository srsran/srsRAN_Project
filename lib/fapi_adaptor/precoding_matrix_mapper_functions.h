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

namespace srsran {
namespace fapi_adaptor {

/// Returns the precoding matrix index for the SSB codebook.
inline unsigned get_ssb_precoding_matrix_index()
{
  return 0U;
}

/// Returns the precoding matrix index for the CSI-RS codebook.
inline unsigned get_csi_rs_precoding_matrix_index()
{
  return 0U;
}

/// Returns the precoding matrix index for the PDCCH codebook.
inline unsigned get_pdcch_precoding_matrix_index()
{
  return 0U;
}

/// Returns the precoding matrix index for the one-port PDSCH codebook.
inline unsigned get_pdsch_one_port_precoding_matrix_index()
{
  return 0U;
}

/// Returns the precoding matrix index for the PDSCH omnidirectional codebook.
inline unsigned get_pdsch_omnidirectional_precoding_matrix_index()
{
  return 0U;
}

/// Returns the precoding matrix index for the two-port PDSCH codebook using the given PMI.
inline unsigned get_pdsch_two_port_precoding_matrix_index(unsigned pmi)
{
  return pmi;
}

/// Returns the precoding matrix index for the four-port PDSCH codebook using the given i_1_1, i_1_3 and i_1_2 PMI
/// parameters.
inline unsigned get_pdsch_four_port_precoding_matrix_index(unsigned i_1_1, unsigned i_1_3, unsigned i_2)
{
  static constexpr unsigned MAX_NUM_I_1_3 = 2U;
  static constexpr unsigned MAX_NUM_I_2   = 4U;

  return MAX_NUM_I_2 * MAX_NUM_I_1_3 * i_1_1 + MAX_NUM_I_2 * i_1_3 + i_2;
}

} // namespace fapi_adaptor
} // namespace srsran
