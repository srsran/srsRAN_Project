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

#include "ofh_cplane_message_builder_impl.h"

namespace srsran {
namespace ofh {

/// Open Fronthaul Control-Plane message builder for static compression.
class cplane_message_builder_static_compression_impl : public cplane_message_builder_impl
{
private:
  // See interface for documentation
  void serialize_compression_header(network_order_binary_serializer& serializer,
                                    const ru_compression_params&     compr,
                                    data_direction                   direction) const override;
};

} // namespace ofh
} // namespace srsran
