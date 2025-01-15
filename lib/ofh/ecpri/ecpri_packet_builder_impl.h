/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
