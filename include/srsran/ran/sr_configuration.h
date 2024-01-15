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

#include "srsran/support/error_handling.h"
#include <cstdint>

namespace srsran {

/// Maximum number of SR resources per BWP in a cell.
/// \remark See TS 38.331, "maxNrofSR-Resources".
constexpr size_t MAX_NOF_SR_RESOURCES = 8;

/// \c SchedulingRequestResourceId, as per TS 38.331.
enum scheduling_request_id : uint8_t { SR_ID_MIN = 0, SR_ID_MAX = 7 };

/// Convert unsigned to scheduling_request_id.
inline scheduling_request_id uint_to_sched_req_id(unsigned sr_id)
{
  return static_cast<scheduling_request_id>(sr_id);
}

/// Encodes the periodicity (only) of \c periodicityAndOffset for \c SchedulingRequestResourceConfig, TS 38.331. Note
/// that the offset is encoded separately.
enum class sr_periodicity : int {
  sym_2      = -2,
  sym_6_or_7 = -1,
  sl_1       = 1,
  sl_2       = 2,
  sl_4       = 4,
  sl_5       = 5,
  sl_8       = 8,
  sl_10      = 10,
  sl_16      = 16,
  sl_20      = 20,
  sl_40      = 40,
  sl_80      = 80,
  sl_160     = 160,
  sl_320     = 320,
  sl_640     = 640
};

/// Convert unsigned to scheduling_request_id.
inline unsigned sr_periodicity_to_slot(sr_periodicity period)
{
  if (period == sr_periodicity::sym_2 or period == sr_periodicity::sym_6_or_7) {
    return static_cast<unsigned>(sr_periodicity::sl_1);
  }
  return static_cast<unsigned>(period);
}

/// \c sr-ProhibitTimer possible values, for \c SchedulingRequestToAddMod, TS 38.331.
enum class sr_prohib_timer { ms1, ms2, ms4, ms8, ms16, ms32, ms64, ms128 };

/// Return the enum value of \ref sr-ProhibitTimer corresponding to the given unsigned value.
inline sr_prohib_timer to_sr_prohib_timer(unsigned sr_prohibit_timer)
{
  switch (sr_prohibit_timer) {
    case 1:
      return sr_prohib_timer::ms1;
    case 2:
      return sr_prohib_timer::ms2;
    case 4:
      return sr_prohib_timer::ms4;
    case 8:
      return sr_prohib_timer::ms8;
    case 16:
      return sr_prohib_timer::ms16;
    case 32:
      return sr_prohib_timer::ms32;
    case 64:
      return sr_prohib_timer::ms64;
    case 128:
      return sr_prohib_timer::ms128;
    default:
      report_fatal_error("Invalid sr-ProhibitTimer value={}", sr_prohibit_timer);
  }
}

/// \c sr-TransMax possible values, for \c SchedulingRequestToAddMod, TS 38.331.
enum class sr_max_tx { n4 = 4, n8 = 8, n16 = 16, n32 = 32, n64 = 64 };

/// Return the enum value of \ref sr-TransMax corresponding to the given unsigned value.
inline sr_max_tx to_sr_max_tx(unsigned max_tx)
{
  switch (max_tx) {
    case 4:
      return sr_max_tx::n4;
    case 8:
      return sr_max_tx::n8;
    case 16:
      return sr_max_tx::n16;
    case 32:
      return sr_max_tx::n32;
    case 64:
      return sr_max_tx::n64;
    default:
      report_fatal_error("Invalid sr-TransMax value={}", max_tx);
  }
}

/// \c SchedulingRequestToAddMod, part of \c SchedulingRequestConfig, TS 38.331.
struct scheduling_request_to_addmod {
  /// \c schedulingRequestId.
  scheduling_request_id     sr_id;
  optional<sr_prohib_timer> prohibit_timer;
  sr_max_tx                 max_tx;

  bool operator==(const scheduling_request_to_addmod& rhs) const
  {
    return sr_id == rhs.sr_id && prohibit_timer == rhs.prohibit_timer && max_tx == rhs.max_tx;
  }
  bool operator!=(const scheduling_request_to_addmod& rhs) const { return !(rhs == *this); }
};

/// SR Configuration, as per \c SchedulingRequestResourceConfig, TS 38.331.
struct scheduling_request_resource_config {
  bool operator==(const scheduling_request_resource_config& rhs) const
  {
    return sr_res_id == rhs.sr_res_id && sr_id == rhs.sr_id && period == rhs.period && offset == rhs.offset &&
           pucch_res_id == rhs.pucch_res_id;
  }
  bool operator!=(const scheduling_request_resource_config& rhs) const { return !(rhs == *this); }

  /// \c schedulingRequestResourceId, which identifies this SchedulingRequestResourceConfig.
  unsigned sr_res_id;
  /// \c schedulingRequest, which is the ID of \ref scheduling_request_to_addmod that uses this resource.
  scheduling_request_id sr_id;
  /// \c periodicityAndOffset is here represented separately as pediod and offset.
  sr_periodicity period;
  unsigned       offset;
  /// \c resource, which identifies the PUCCH resource to be used for this SR.
  unsigned pucch_res_id;
};

} // namespace srsran
