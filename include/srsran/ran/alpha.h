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

/// The IE Alpha defines possible values of a the pathloss compensation coefficient for uplink power control. Value
/// alpha0 corresponds to the value 0, Value alpha04 corresponds to the value 0.4, and so on. Value alpha1 corresponds
/// to value 1.
/// \remark See TS 38.331, Alpha. Also, see clause 7.1 of TS 38.213.
enum class alpha : unsigned { alpha0, alpha04, alpha05, alpha06, alpha07, alpha08, alpha09, alpha1, not_set };

} // namespace srsran
