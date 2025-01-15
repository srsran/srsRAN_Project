/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
  constexpr static std::array<const char*, 4> names = {"1_0", "1_1", "2_0", "invalid"};
  unsigned                                    idx   = static_cast<unsigned>(type);
  return names[idx < names.size() ? idx : names.size() - 1];
}

inline const char* dci_ul_format_to_string(dci_ul_format type)
{
  constexpr static std::array<const char*, 3> names = {"0_0", "0_1", "invalid"};
  unsigned                                    idx   = static_cast<unsigned>(type);
  return names[idx < names.size() ? idx : names.size() - 1];
}

} // namespace srsran
