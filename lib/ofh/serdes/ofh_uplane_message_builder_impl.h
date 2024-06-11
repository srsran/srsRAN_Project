/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/ofh/serdes/ofh_uplane_message_builder.h"
#include "srsran/srslog/logger.h"

namespace srsran {
namespace ofh {

class iq_compressor;
class network_order_binary_serializer;

/// Open Fronthaul User-Plane message builder implementation.
class uplane_message_builder_impl : public uplane_message_builder
{
public:
  uplane_message_builder_impl(srslog::basic_logger& logger_, iq_compressor& compressor_) :
    logger(logger_), compressor(compressor_)
  {
  }

  // See interface for documentation.
  unsigned build_message(span<uint8_t> buffer, span<const cf_t> iq_data, const uplane_message_params& params) override;

private:
  /// Serializes IQ data from a resource grid.
  void serialize_iq_data(network_order_binary_serializer& serializer,
                         span<const cf_t>                 iq_data,
                         unsigned                         nof_prbs,
                         const ru_compression_params&     params);

  /// Serializes compression header. Implementation depends on whether static or non-static IQ format is configured.
  virtual void serialize_compression_header(network_order_binary_serializer& serializer,
                                            const ru_compression_params&     params) = 0;

protected:
  const bool            ud_comp_length_support = false;
  srslog::basic_logger& logger;
  iq_compressor&        compressor;
};

} // namespace ofh
} // namespace srsran
