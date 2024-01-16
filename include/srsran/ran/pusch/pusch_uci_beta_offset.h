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

namespace srsran {

/// \brief Maps the integer \c beta_offset value for HARQ-ACK reporting into the corresponding float value.
///
/// The integer \c beta_offset value is passed by the \c PUSCH-Config, TS 38.331, and the mapping into the corresponding
/// float value is determined as per Table 9.3-1, TS 38.213.
/// \param[in] beta_uint_val is the integer value as per \c BetaOffsets, TS 38.331.
/// \remark The caller must ensure \c beta_uint_val < 16, so as not to incur in reserved values, as per Table 9.3-1,
/// TS 38.213.
/// \return The corresponding float value as per Table 9.3-1, TS 38.213.
float beta_harq_ack_to_float(unsigned beta_uint_val);

/// \brief Maps the integer \c beta_offset value for CSI reporting into the corresponding float value.
///
/// The integer \c beta_offset value is passed by the \c PUSCH-Config, TS 38.331, and the mapping into the corresponding
/// float value is determined as per Table 9.3-2, TS 38.213.
/// \param[in] beta_uint_val is the integer value as per \c BetaOffsets, TS 38.331.
/// \remark The caller must ensure \c beta_uint_val < 19, so as not to incur in reserved values, as per Table 9.3-2,
/// TS 38.213.
/// \return The corresponding float value as per Table 9.3-2, TS 38.213.
float beta_csi_to_float(unsigned beta_uint_val);

} // namespace srsran
