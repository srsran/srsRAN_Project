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

/// \c SchedulingRequestResourceId, as per TS 38.331.
enum scheduling_request_resource_id : uint8_t { SR_ID_MIN = 0, SR_ID_MAX = 7 };

/// Convert unsigned to scheduling_request_resource_id.
inline scheduling_request_resource_id uint_to_sched_req_id(unsigned sr_id)
{
  return static_cast<scheduling_request_resource_id>(sr_id);
}

/// Encodes the periodicity (only) of \c periodicityAndOffset for \c SchedulingRequestResourceConfig, TS 38.331. Note
/// that the offset is encoded separately.
enum class sr_periodicity {
  sym_2,
  sym_6_or_7,
  sl_1,
  sl_2,
  sl_4,
  sl_5,
  sl_8,
  sl_10,
  sl_16,
  sl_20,
  sl_40,
  sl_80,
  sl_160,
  sl_320,
  sl_640
};

/// \c sr-ProhibitTimer possible values, for \c SchedulingRequestToAddMod, TS 38.331.
enum class sr_prohib_timer { ms1, ms2, ms4, ms8, ms16, ms32, ms64, ms128 };

/// \c sr-TransMax possible values, for \c SchedulingRequestToAddMod, TS 38.331.
enum class sr_max_tx { n4, n8, n16, n32, n64 };

/// \c SchedulingRequestToAddMod, part of \c SchedulingRequestConfig, TS 38.331.
struct scheduling_request_to_addmod {
  /// \c schedulingRequestId.
  scheduling_request_resource_id sr_id;
  optional<sr_prohib_timer>      prohibit_timer;
  sr_max_tx                      max_tx;
};

/// SR Configuration, as per \c SchedulingRequestResourceConfig, TS 38.331.
struct scheduling_request_resource_config {
  /// \c schedulingRequestResourceId, which identifies this SchedulingRequestResourceConfig.
  unsigned sr_res_id;
  /// \c schedulingRequest, which is the ID of \ref scheduling_request_to_addmod that uses this resource.
  scheduling_request_resource_id sr_id;
  /// \c periodicityAndOffset is here represented separately as pediod and offset.
  sr_periodicity period;
  unsigned       offset;
  /// \c resource, which identifies the PUCCH resource to be used for this SR.
  unsigned pucch_res_id;
};

} // namespace srsgnb
