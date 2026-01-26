/*
 *
 * Copyright 2021-2026 Software Radio Systems Limited
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

#include <cstdint>

namespace srsran {

/// \brief Configuration of group and sequence hopping as described in TS38.331 PUCCH-ConfigCommon IE.
enum class pucch_group_hopping {
  /// Implies neither group or sequence hopping is enabled.
  NEITHER,
  /// Enables group hopping and disables sequence hopping.
  ENABLE,
  /// Disables group hopping and enables sequence hopping.
  DISABLE
};

/// \brief PUCCH resource set index, as per \c PUCCH-ResourceSetId, TS 38.331.
/// At the moment, we only supports PUCCH resource set index 0 and 1.
enum class pucch_res_set_idx : uint8_t { set_0 = 0, set_1 };

inline uint8_t pucch_res_set_idx_to_uint(pucch_res_set_idx format)
{
  return static_cast<uint8_t>(format);
}

/// \brief PUCCH Formats as described in TS38.213 Section 9.2.
enum class pucch_format : uint8_t { FORMAT_0, FORMAT_1, FORMAT_2, FORMAT_3, FORMAT_4, NOF_FORMATS };

inline uint8_t pucch_format_to_uint(pucch_format format)
{
  return static_cast<uint8_t>(format);
}

inline const char* to_string(pucch_format format)
{
  switch (format) {
    case pucch_format::FORMAT_0:
      return "FORMAT_0";
    case pucch_format::FORMAT_1:
      return "FORMAT_1";
    case pucch_format::FORMAT_2:
      return "FORMAT_2";
    case pucch_format::FORMAT_3:
      return "FORMAT_3";
    case pucch_format::FORMAT_4:
      return "FORMAT_4";
    default:
      return "UNKNOWN";
  }
}

/// Defines the allowed combinations of PUCCH formats to use for the resources in a given cell.
enum class pucch_formats { f0_and_f2, f1_and_f2, f1_and_f3, f1_and_f4 };

inline const char* to_string(pucch_formats formats)
{
  switch (formats) {
    case pucch_formats::f0_and_f2:
      return "f0_and_f2";
    case pucch_formats::f1_and_f2:
      return "f1_and_f2";
    case pucch_formats::f1_and_f3:
      return "f1_and_f3";
    case pucch_formats::f1_and_f4:
      return "f1_and_f4";
    default:
      return "UNKNOWN";
  }
}

/// \brief Returns a format between F0 and F1 for the given combination of PUCCH formats.
/// This format is used for Resource Set ID 0 and SR resources.
inline pucch_format pucch_f0f1_format(pucch_formats formats)
{
  switch (formats) {
    case pucch_formats::f0_and_f2:
      return pucch_format::FORMAT_0;
    case pucch_formats::f1_and_f2:
    case pucch_formats::f1_and_f3:
    case pucch_formats::f1_and_f4:
      return pucch_format::FORMAT_1;
    default:
      return pucch_format::NOF_FORMATS;
  }
}

/// Returns a format between F2, F3 and F4 for the given combination of PUCCH formats.
/// This format is used for Resource Set ID 1 and CSI resources.
inline pucch_format pucch_f2f3f4_format(pucch_formats formats)
{
  switch (formats) {
    case pucch_formats::f0_and_f2:
    case pucch_formats::f1_and_f2:
      return pucch_format::FORMAT_2;
    case pucch_formats::f1_and_f3:
      return pucch_format::FORMAT_3;
    case pucch_formats::f1_and_f4:
      return pucch_format::FORMAT_4;
    default:
      return pucch_format::NOF_FORMATS;
  }
}

/// Defines whether the PUCCH within the current slot belongs to a PUCCH repetition. For more details, refer to
/// TS38.213, Section 9.2.6.
enum class pucch_repetition_tx_slot { no_multi_slot, starts, continues, ends };

/// \brief Defines a couple of PUCCH resource IDs, the cell's and UE's resource PUCCH resource ID, respectively.
struct pucch_res_id_t {
  /// This is the PUCCH cell resource ID and is used by the DU and by the scheduler to identify the PUCCH resource.
  unsigned cell_res_id;
  /// Corresponds to \c pucch-ResourceId, as part of \c PUCCH-Resource, in \c PUCCH-Config, TS 38.331.
  /// Used by the DU only to populate the ASN1 message, while it's not used by the scheduler.
  unsigned ue_res_id;

  bool operator==(const pucch_res_id_t& rhs) const
  {
    return cell_res_id == rhs.cell_res_id && ue_res_id == rhs.ue_res_id;
  }
  bool operator!=(const pucch_res_id_t& rhs) const { return !(rhs == *this); }
};

} // namespace srsran
