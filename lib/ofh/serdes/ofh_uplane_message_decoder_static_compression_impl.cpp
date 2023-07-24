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

#include "ofh_uplane_message_decoder_static_compression_impl.h"
#include "../support/network_order_binary_deserializer.h"

using namespace srsran;
using namespace ofh;

bool uplane_message_decoder_static_compression_impl::decode_compression_header(
    uplane_section_params&             results,
    network_order_binary_deserializer& deserializer,
    bool                               is_a_prach_msg)
{
  // Copy the compression header to the results.
  results.ud_comp_hdr = (is_a_prach_msg ? prach_compression_params : compression_params);

  return true;
}

uplane_message_decoder_static_compression_impl::uplane_message_decoder_static_compression_impl(
    srslog::basic_logger&        logger_,
    subcarrier_spacing           scs_,
    unsigned                     nof_symbols_,
    unsigned                     ru_nof_prbs_,
    iq_decompressor&             decompressor_,
    const ru_compression_params& compression_params_,
    const ru_compression_params& prach_compression_params_) :
  uplane_message_decoder_impl(logger_, scs_, nof_symbols_, ru_nof_prbs_, decompressor_),
  compression_params(compression_params_),
  prach_compression_params(prach_compression_params_)
{
}
