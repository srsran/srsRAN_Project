/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include <array>

namespace srsran {

/// Supported DL DCI formats.
enum class dci_dl_format { f1_0, f1_1, f2_0 };

/// Supported UL DCI formats.
enum class dci_ul_format { f0_0, f0_1 };

inline const char* dci_dl_format_to_string(dci_dl_format type)
{
  static constexpr std::array<const char*, 4> names = {"1_0", "1_1", "2_0", "invalid"};
  unsigned                                    idx   = static_cast<unsigned>(type);
  return names[idx < names.size() ? idx : names.size() - 1];
}

inline const char* dci_ul_format_to_string(dci_ul_format type)
{
  static constexpr std::array<const char*, 3> names = {"0_0", "0_1", "invalid"};
  unsigned                                    idx   = static_cast<unsigned>(type);
  return names[idx < names.size() ? idx : names.size() - 1];
}

} // namespace srsran
