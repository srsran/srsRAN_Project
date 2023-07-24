/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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
#include "srsran/ran/prach/prach_subcarrier_spacing.h"
#include "srsran/ran/slot_point.h"

namespace srsran {
namespace ofh {

/// Parameters used to build Open Fronthaul PRACH Control-Plane message of section type 3.
struct cplane_scheduling_prach_context {
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

/// Open Fronthaul data flow for Control-Plane scheduling and beamforming commands.
class data_flow_cplane_scheduling_commands
{
public:
  /// Default destructor.
  virtual ~data_flow_cplane_scheduling_commands() = default;

  /// Enqueues Open Fronthaul section type 1 message with the given slot and eAxC.
  virtual void enqueue_section_type_1_message(slot_point        slot,
                                              unsigned          eaxc,
                                              data_direction    direction,
                                              filter_index_type filter_type) = 0;

  /// Enqueues Open Fronthaul section type 3 PRACH message with the given parameters.
  virtual void enqueue_section_type_3_prach_message(slot_point                             slot,
                                                    unsigned                               eaxc,
                                                    filter_index_type                      filter_type,
                                                    const cplane_scheduling_prach_context& context) = 0;
};

} // namespace ofh
} // namespace srsran
