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

#include "srsran/ofh/ecpri/ecpri_packet_builder.h"

namespace srsran {
namespace ecpri {

/// eCPRI packet builder implementation.
class packet_builder_impl : public packet_builder
{
public:
  // See interface for documentation.
  units::bytes get_header_size(message_type msg_type) const override;

  // See interface for documentation.
  void build_control_packet(span<uint8_t> buffer, const realtime_control_parameters& msg_params) override;

  // See interface for documentation.
  void build_data_packet(span<uint8_t> buffer, const iq_data_parameters& msg_params) override;
};

} // namespace ecpri
} // namespace srsran
