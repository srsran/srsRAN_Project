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

#include "../../../../lib/ofh/transmitter/ofh_data_flow_cplane_scheduling_commands.h"

namespace srsran {
namespace ofh {
namespace testing {

/// Spy Control-Plane scheduling commands data flow.
class data_flow_cplane_scheduling_commands_spy : public data_flow_cplane_scheduling_commands
{
public:
  struct spy_info {
    unsigned          eaxc      = -1;
    data_direction    direction = data_direction::downlink;
    slot_point        slot;
    filter_index_type filter_type;
  };

  void enqueue_section_type_1_message(slot_point        slot,
                                      unsigned          eaxc,
                                      data_direction    direction,
                                      filter_index_type filter_type) override
  {
    has_enqueue_section_type_1_message_method_been_called = true;
    info.slot                                             = slot;
    info.eaxc                                             = eaxc;
    info.direction                                        = direction;
    info.filter_type                                      = filter_type;
  }

  void enqueue_section_type_3_prach_message(slot_point                                    slot,
                                            unsigned                                      eaxc,
                                            filter_index_type                             filter_type,
                                            const struct cplane_scheduling_prach_context& context) override
  {
    has_enqueue_section_type_3_message_method_been_called = true;
    info.slot                                             = slot;
    info.eaxc                                             = eaxc;
    info.direction                                        = data_direction::uplink;
    info.filter_type                                      = filter_type;
  }

  /// Returns true if the method enqueue section type 1 message has been called, otherwise false.
  bool has_enqueue_section_type_1_method_been_called() const
  {
    return has_enqueue_section_type_1_message_method_been_called;
  }

  /// Returns true if the method enqueue section type 3 message has been called, otherwise false.
  bool has_enqueue_section_type_3_method_been_called() const
  {
    return has_enqueue_section_type_3_message_method_been_called;
  }

  /// Returns the configured eAxC.
  spy_info get_spy_info() const { return info; }

private:
  bool     has_enqueue_section_type_1_message_method_been_called = false;
  bool     has_enqueue_section_type_3_message_method_been_called = false;
  spy_info info;
};

} // namespace testing
} // namespace ofh
} // namespace srsran