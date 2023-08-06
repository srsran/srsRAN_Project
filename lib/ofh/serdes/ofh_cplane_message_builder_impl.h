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

#include "srsran/ofh/serdes/ofh_cplane_message_builder.h"

namespace srsran {
namespace ofh {

class network_order_binary_serializer;

/// Open Fronthaul Control-Plane message builder implementation.
class cplane_message_builder_impl : public cplane_message_builder
{
public:
  // See interface for documentation.
  unsigned build_dl_ul_radio_channel_message(span<uint8_t>                          buffer,
                                             const cplane_section_type1_parameters& msg_params) override;

  // See interface for documentation.
  unsigned build_idle_guard_period_message(span<uint8_t>                          buffer,
                                           const cplane_section_type0_parameters& msg_params) override;

  // See interface for documentation.
  unsigned build_prach_mixed_numerology_message(span<uint8_t>                          buffer,
                                                const cplane_section_type3_parameters& msg_params) override;

private:
  /// Serializes the compression header using the given serializer, compression parameters and direction.
  virtual void serialize_compression_header(network_order_binary_serializer& serializer,
                                            const ru_compression_params&     compr,
                                            data_direction                   direction) const = 0;
};

} // namespace ofh
} // namespace srsran
