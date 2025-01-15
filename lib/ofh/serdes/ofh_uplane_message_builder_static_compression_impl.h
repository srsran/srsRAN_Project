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

#include "ofh_uplane_message_builder_impl.h"

namespace srsran {
namespace ofh {

class iq_compressor;
class network_order_binary_serializer;

/// Open Fronthaul User-Plane message builder implementation with static IQ compression.
class ofh_uplane_message_builder_static_compression_impl : public uplane_message_builder_impl
{
public:
  ofh_uplane_message_builder_static_compression_impl(srslog::basic_logger& logger_, iq_compressor& compressor_) :
    uplane_message_builder_impl(logger_, compressor_)
  {
  }

  // See interface for documentation.
  units::bytes get_header_size(const ru_compression_params& params) const override;

private:
  // See parent for documentation.
  void serialize_compression_header(network_order_binary_serializer& serializer,
                                    const ru_compression_params&     params) override;
};

} // namespace ofh
} // namespace srsran
