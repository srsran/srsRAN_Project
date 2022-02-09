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

#include "pbch_encode_test_data.h"
#include "srsgnb/phy/channel_processors/pbch_encoder.h"

using namespace srsgnb;

int main()
{
  std::unique_ptr<pbch_encoder> encoder = create_pbch_encoder();

  for (const test_case_t& test_case : pbch_encode_test_data) {
    // Encode message
    std::array<uint8_t, pbch_encoder::E> encoded_data = {};
    encoder->encode(test_case.pbch_msg, encoded_data);

    // Assert encoded data
    for (unsigned i = 0; i != pbch_encoder::E; ++i) {
      assert(encoded_data[i] == test_case.endoded[i]);
    }
  }
  return 0;
}