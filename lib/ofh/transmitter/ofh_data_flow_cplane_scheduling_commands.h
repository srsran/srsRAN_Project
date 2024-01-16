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

#include "srsran/ofh/serdes/ofh_message_properties.h"
#include "srsran/ran/ofdm_symbol_range.h"
#include "srsran/ran/prach/prach_subcarrier_spacing.h"
#include "srsran/ran/slot_point.h"

namespace srsran {
namespace ofh {

/// Open Fronthaul Control-Plane PRACH context parameters.
struct data_flow_cplane_scheduling_prach_context {
  /// Slot point.
  slot_point slot;
  /// eAxC.
  unsigned eaxc;
  /// Filter type.
  filter_index_type filter_type;
  /// This parameter is the last symbol that starts right at or before the PRACH preamble (after cyclic prefix).
  unsigned start_symbol;
  /// Number of PRACH repetitions, used to initialize numSymbol parameter.
  unsigned nof_repetitions;
  /// PUSCH subcarrier spacing.
  subcarrier_spacing scs;
  /// PRACH subcarrier spacing.
  prach_subcarrier_spacing prach_scs;
  /// First subcarrier index used by PRACH message.
  unsigned prach_start_re;
  /// Number of PRBs used by PRACH.
  unsigned prach_nof_rb;
  /// Time offset to the start of PRACH.
  unsigned time_offset;
};

/// Open Fronthaul Control-Plane type 1 context parameters.
struct data_flow_cplane_type_1_context {
  /// Slot point.
  slot_point slot;
  /// eAxC.
  unsigned eaxc;
  /// Filter type.
  filter_index_type filter_type;
  /// Direction.
  data_direction direction;
  /// Symbol range.
  ofdm_symbol_range symbol_range;
};

/// Open Fronthaul Control-Plane scheduling and beamforming commands data flow.
class data_flow_cplane_scheduling_commands
{
public:
  /// Default destructor.
  virtual ~data_flow_cplane_scheduling_commands() = default;

  /// Enqueues Open Fronthaul section type 1 message with the given slot and eAxC.
  virtual void enqueue_section_type_1_message(const data_flow_cplane_type_1_context& context) = 0;

  /// Enqueues Open Fronthaul section type 3 PRACH message with the given parameters.
  virtual void enqueue_section_type_3_prach_message(const data_flow_cplane_scheduling_prach_context& context) = 0;
};

} // namespace ofh
} // namespace srsran
