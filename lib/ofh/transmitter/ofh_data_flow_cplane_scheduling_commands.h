/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/ofh/ofh_message_properties.h"
#include "srsran/ran/slot_point.h"

namespace srsran {
namespace ofh {

/// Open Fronthaul data flow for Control-Plane scheduling and beamforming commands.
class data_flow_cplane_scheduling_commands
{
public:
  /// Default destructor.
  virtual ~data_flow_cplane_scheduling_commands() = default;

  /// Enqueues a section type 1 Open Fronthaul message with the given slot and eAxC.
  virtual void enqueue_section_type_1_message(slot_point        slot,
                                              unsigned          eaxc,
                                              data_direction    direction,
                                              filter_index_type filter_type) = 0;
};

} // namespace ofh
} // namespace srsran
