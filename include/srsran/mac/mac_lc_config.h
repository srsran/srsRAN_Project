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

#include "srsran/ran/sr_configuration.h"
#include "srsran/scheduler/config/logical_channel_group.h"
#include "srsran/support/error_handling.h"

namespace srsran {

/// Prioritized Bit rate value in kiloBytes/s. See TS 38.331, \c prioritisedBitRate.
enum class prioritized_bit_rate {
  kBps0     = 0,
  kBps8     = 8,
  kBps16    = 16,
  kBps32    = 32,
  kBps64    = 64,
  kBps128   = 128,
  kBps256   = 256,
  kBps512   = 512,
  kBps1024  = 1024,
  kBps2048  = 2048,
  kBps4096  = 4096,
  kBps8192  = 8192,
  kBps16384 = 16384,
  kBps32768 = 32768,
  kBps65536 = 65536,
  infinity  = 65537,
};

/// Returns the \c prioritized_bit_rate enum value corresponding to the given unsigned value.
inline prioritized_bit_rate to_prioritized_bit_rate(unsigned pbr)
{
  switch (pbr) {
    case 0:
      return prioritized_bit_rate::kBps0;
    case 8:
      return prioritized_bit_rate::kBps8;
    case 16:
      return prioritized_bit_rate::kBps16;
    case 32:
      return prioritized_bit_rate::kBps32;
    case 64:
      return prioritized_bit_rate::kBps64;
    case 128:
      return prioritized_bit_rate::kBps128;
    case 256:
      return prioritized_bit_rate::kBps256;
    case 512:
      return prioritized_bit_rate::kBps512;
    case 1024:
      return prioritized_bit_rate::kBps1024;
    case 2048:
      return prioritized_bit_rate::kBps2048;
    case 4096:
      return prioritized_bit_rate::kBps4096;
    case 8192:
      return prioritized_bit_rate::kBps8192;
    case 16384:
      return prioritized_bit_rate::kBps16384;
    case 32768:
      return prioritized_bit_rate::kBps32768;
    case 65536:
      return prioritized_bit_rate::kBps65536;
    case 65537:
      return prioritized_bit_rate::infinity;
    default:
      report_fatal_error("Invalid prioritisedBitRate value={}", pbr);
  }
}

/// Bucket size duration in milliseconds. See TS 38.331, \c bucketSizeDuration.
enum class bucket_size_duration {
  ms5    = 5,
  ms10   = 10,
  ms20   = 20,
  ms50   = 50,
  ms100  = 100,
  ms150  = 150,
  ms300  = 300,
  ms500  = 500,
  ms1000 = 1000,
  spare7 = 1007,
  spare6 = 1006,
  spare5 = 1005,
  spare4 = 1004,
  spare3 = 1003,
  spare2 = 1002,
  spare1 = 1001,
};

/// Returns the \c bucket_size_duration enum value corresponding to the given unsigned value.
inline bucket_size_duration to_bucket_size_duration(unsigned bsd)
{
  switch (bsd) {
    case 5:
      return bucket_size_duration::ms5;
    case 10:
      return bucket_size_duration::ms10;
    case 20:
      return bucket_size_duration::ms20;
    case 50:
      return bucket_size_duration::ms50;
    case 100:
      return bucket_size_duration::ms100;
    case 150:
      return bucket_size_duration::ms150;
    case 300:
      return bucket_size_duration::ms300;
    case 500:
      return bucket_size_duration::ms500;
    case 1000:
      return bucket_size_duration::ms1000;
    default:
      report_fatal_error("Invalid bucketSizeDuration value={}", bsd);
  }
}

/// Configurations for MAC logical channel parameters.
struct mac_lc_config {
  /// Logical channel priority, as specified in TS 38.321. An increasing priority value indicates a lower priority
  /// level. Values {1,...,16}.
  uint8_t priority;
  /// Prioritized Bit Rate. For SRBs, the value can only be set to infinity.
  prioritized_bit_rate pbr;
  /// Bucket size duration.
  bucket_size_duration bsd;
  /// ID of the logical channel group, as specified in TS 38.321, which the logical channel belongs to.
  lcg_id_t lcg_id;
  /// Controls SR triggering when a configured uplink grant of type1 or type2 is configured. TRUE indicates that SR
  /// masking is configured for this logical channel as specified in TS 38.321
  bool lc_sr_mask;
  /// Indicates the scheduling request configuration applicable for this logical channel.
  scheduling_request_id sr_id;
  /// Indicates whether to apply the delay timer for SR transmission for this logical channel. Set to FALSE if
  /// logicalChannelSR-DelayTimer is not included in BSR-Config.
  bool lc_sr_delay_applied;

  bool operator==(const mac_lc_config& rhs) const
  {
    return priority == rhs.priority && pbr == rhs.pbr && bsd == rhs.bsd && lcg_id == rhs.lcg_id &&
           lc_sr_mask == rhs.lc_sr_mask && sr_id == rhs.sr_id && lc_sr_delay_applied == rhs.lc_sr_delay_applied;
  }
  bool operator!=(const mac_lc_config& rhs) const { return !(rhs == *this); }
};

} // namespace srsran
