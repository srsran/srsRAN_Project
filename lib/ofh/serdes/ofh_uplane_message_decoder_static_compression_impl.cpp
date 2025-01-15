/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ofh_uplane_message_decoder_static_compression_impl.h"
#include "../support/network_order_binary_deserializer.h"

using namespace srsran;
using namespace ofh;

uplane_message_decoder_impl::decoded_section_status
uplane_message_decoder_static_compression_impl::decode_compression_header(
    decoder_uplane_section_params&     results,
    network_order_binary_deserializer& deserializer)
{
  // Copy the compression header to the results.
  results.ud_comp_hdr = compression_params;

  return uplane_message_decoder_impl::decoded_section_status::ok;
}

uplane_message_decoder_static_compression_impl::uplane_message_decoder_static_compression_impl(
    srslog::basic_logger&            logger_,
    subcarrier_spacing               scs_,
    unsigned                         nof_symbols_,
    unsigned                         ru_nof_prbs_,
    unsigned                         sector_id_,
    std::unique_ptr<iq_decompressor> decompressor_,
    const ru_compression_params&     compression_params_) :
  uplane_message_decoder_impl(logger_, scs_, nof_symbols_, ru_nof_prbs_, sector_id_, std::move(decompressor_)),
  compression_params(compression_params_)
{
}
