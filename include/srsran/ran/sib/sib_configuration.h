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

/// \brief SIB retransmission periodicity in milliseconds as per TS38.331 Section 5.2.1.
/// \remark This is used for retransmission periodicity only; SIB1 periodicity is given as 160ms.
enum class sib1_rtx_periodicity { ms5 = 5, ms10 = 10, ms20 = 20, ms40 = 40, ms80 = 80, ms160 = 160 };

/// \brief Converts the SIB1 periodicity property to its corresponding value in milliseconds.
inline unsigned sib1_rtx_periodicity_to_value(sib1_rtx_periodicity periodicity)
{
  return static_cast<unsigned>(periodicity);
}

} // namespace srsran
