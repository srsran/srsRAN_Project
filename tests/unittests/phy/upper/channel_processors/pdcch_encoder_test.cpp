/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "pdcch_encoder_test_data.h"
#include "srsran/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsran/support/srsran_test.h"

using namespace srsran;

int main()
{
  std::array<uint8_t, pdcch_constants::MAX_NOF_BITS> encoded_data = {};

  span<uint8_t> encoded_data_span{encoded_data};

  std::shared_ptr<crc_calculator_factory> crc_factory = create_crc_calculator_factory_sw("auto");
  TESTASSERT(crc_factory);

  std::shared_ptr<polar_factory> encoder_factory = create_polar_factory_sw();
  TESTASSERT(encoder_factory);

  std::shared_ptr<pdcch_encoder_factory> pdcch_factory = create_pdcch_encoder_factory_sw(crc_factory, encoder_factory);
  TESTASSERT(pdcch_factory);

  // Create PDCCH Encoder instance
  std::unique_ptr<pdcch_encoder> encoder = pdcch_factory->create();
  TESTASSERT(encoder);

  for (const test_case_t& test_case : pdcch_encoder_test_data) {
    // Load input data
    const std::vector<uint8_t> testvector_data = test_case.message.read();

    // Encode message
    auto encoded_msg = encoded_data_span.first(test_case.config.E);
    encoder->encode(encoded_msg, testvector_data, test_case.config);

    // Load output golden data
    const std::vector<uint8_t> testvector_encoded = test_case.encoded.read();

    // Assert encoded data.
    for (unsigned i = 0; i != test_case.config.E; ++i) {
      TESTASSERT_EQ(encoded_msg[i], testvector_encoded[i], "Bit index {}.", i);
    }
  }
  return 0;
}