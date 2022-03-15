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

#include "pbch_encoder_test_data.h"
#include "srsgnb/phy/upper/channel_processors/pbch_encoder.h"
#include "srsgnb/support/srsran_assert.h"

using namespace srsgnb;

int main()
{
  std::unique_ptr<pbch_encoder> encoder = create_pbch_encoder();

  for (const test_case_t& test_case : pbch_encoder_test_data) {
    // Encode PBCH message.
    std::array<uint8_t, pbch_encoder::E> encoded_data = {};
    encoder->encode(test_case.pbch_msg, encoded_data);

    // Assert encoded data.
    for (unsigned i = 0; i != pbch_encoder::E; ++i) {
      srsran_assert(encoded_data[i] == test_case.encoded[i], "Failed");
    }
  }
  return 0;
}