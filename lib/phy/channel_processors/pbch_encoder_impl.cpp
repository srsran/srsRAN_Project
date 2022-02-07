#include <pbch_encoder.h>
/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pbch_encoder_impl.h"

using namespace srsgnb;

pbch_encoder_impl::pbch_encoder_impl(args_t& args) : crc24c(std::move(args.crc24c)) {}

void pbch_encoder_impl::encode(const srsgnb::pbch_encoder::pbch_msg_t& pbch_msg, std::array<uint8_t, 864>& encoded) {}
