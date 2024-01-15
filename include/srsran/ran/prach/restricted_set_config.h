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

/// \brief PRACH Restricted set configuration.
///
/// PRACH \f$N_{CS}\f$ restricted set configuration. Used for interpreting TS38.211 Tables 6.3.3.1-5 and 6.3.3.1-6.
enum class restricted_set_config { UNRESTRICTED, TYPE_A, TYPE_B };

inline const char* to_string(restricted_set_config config)
{
  switch (config) {
    case restricted_set_config::TYPE_A:
      return "type-A";
    case restricted_set_config::TYPE_B:
      return "type-B";
    case restricted_set_config::UNRESTRICTED:
    default:
      return "unrestricted";
  }
}

} // namespace srsran