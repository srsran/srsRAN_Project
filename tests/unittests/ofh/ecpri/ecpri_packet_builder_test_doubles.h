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

#include "srsran/adt/static_vector.h"
#include "srsran/ofh/ecpri/ecpri_packet_builder.h"

namespace srsran {
namespace ecpri {
namespace testing {

/// Spy eCPRI packet builder implementation.
class packet_builder_spy : public packet_builder
{
  bool                                                           build_control_packet_method_called = false;
  bool                                                           build_data_packet_method_called    = false;
  static_vector<realtime_control_parameters, MAX_NSYMB_PER_SLOT> ctrl_params;
  static_vector<iq_data_parameters, MAX_NSYMB_PER_SLOT>          data_params;

public:
  // See interface for documentation.
  units::bytes get_header_size(message_type msg_type) const override { return units::bytes(8); }

  // See interface for documentation.
  void build_control_packet(span<uint8_t> buffer, const realtime_control_parameters& msg_params) override
  {
    build_control_packet_method_called = true;
    ctrl_params.push_back(msg_params);
  }

  // See interface for documentation.
  void build_data_packet(span<uint8_t> buffer, const iq_data_parameters& msg_params) override
  {
    build_data_packet_method_called = true;
    data_params.push_back(msg_params);
  }

  /// Returns true if the build control packet method has been called, otherwise false.
  bool has_build_control_packet_method_been_called() const { return build_control_packet_method_called; }

  /// Returns true if the build data packet method has been called, otherwise false.
  bool has_build_data_packet_method_been_called() const { return build_data_packet_method_called; }

  /// Returns the realtime control parameters processed by this builder.
  span<const realtime_control_parameters> get_control_parameters() const { return ctrl_params; }

  /// Returns the IQ data parameters processed by this builder.
  span<const iq_data_parameters> get_data_parameters() const { return data_params; }
};

} // namespace testing
} // namespace ecpri
} // namespace srsran
