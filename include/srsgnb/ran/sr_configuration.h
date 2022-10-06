/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include <cstdint>

namespace srsgnb {

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

/// \c sr-TransMax possible values, for \c SchedulingRequestToAddMod, TS 38.331.
enum class sr_max_tx { n4, n8, n16, n32, n64 };

/// \c SchedulingRequestToAddMod, part of \c SchedulingRequestConfig, TS 38.331.
struct scheduling_request_to_addmod {
  /// \c schedulingRequestId.
  scheduling_request_id     sr_id;
  optional<sr_prohib_timer> prohibit_timer;
  sr_max_tx                 max_tx;
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

} // namespace srsgnb
