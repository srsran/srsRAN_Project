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

#include "srsgnb/phy/channel_processors/pbch_encoder.h"

using namespace srsgnb;

int main()
{
  std::unique_ptr<pbch_encoder> encoder = create_pbch_encoder();

  // Create PBCH message
  pbch_encoder::pbch_msg_t pbch_msg = {};

  // Encode message
  std::array<uint8_t, pbch_encoder::E> encoded_data;
  encoder->encode(pbch_msg, encoded_data);

  return 0;
}