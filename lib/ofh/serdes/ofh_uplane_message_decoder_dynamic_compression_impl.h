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

#include "ofh_uplane_message_decoder_impl.h"
#include "srsran/srslog/logger.h"

namespace srsran {
namespace ofh {

class iq_decompressor;
class network_order_binary_deserializer;

/// Open Fronthaul User-Plane message decoder implementation with dynamic IQ compression.
class uplane_message_decoder_dynamic_compression_impl : public uplane_message_decoder_impl
{
public:
  explicit uplane_message_decoder_dynamic_compression_impl(srslog::basic_logger& logger_,
                                                           subcarrier_spacing    scs_,
                                                           unsigned              nof_symbols_,
                                                           unsigned              ru_nof_prbs_,
                                                           iq_decompressor&      decompressor_) :
    uplane_message_decoder_impl(logger_, scs_, nof_symbols_, ru_nof_prbs_, decompressor_)
  {
  }

private:
  // See parent for documentation.
  bool decode_compression_header(uplane_section_params&             results,
                                 network_order_binary_deserializer& deserializer,
                                 bool                               is_a_prach_msg) override;
};

} // namespace ofh
} // namespace srsran
