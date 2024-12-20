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

#include "ofh_uplane_message_decoder_impl.h"

namespace srsran {
namespace ofh {

class iq_decompressor;
class network_order_binary_deserializer;

/// Open Fronthaul User-Plane message decoder implementation with static IQ compression.
class uplane_message_decoder_static_compression_impl : public uplane_message_decoder_impl
{
public:
  uplane_message_decoder_static_compression_impl(srslog::basic_logger&            logger_,
                                                 subcarrier_spacing               scs_,
                                                 unsigned                         nof_symbols_,
                                                 unsigned                         ru_nof_prbs_,
                                                 unsigned                         sector_id_,
                                                 std::unique_ptr<iq_decompressor> decompressor_,
                                                 const ru_compression_params&     compression_params_);

private:
  // See parent for documentation.
  decoded_section_status decode_compression_header(decoder_uplane_section_params&     results,
                                                   network_order_binary_deserializer& deserializer) override;

private:
  const ru_compression_params compression_params;
};

} // namespace ofh
} // namespace srsran
