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

#include "pbch_encoder_test_data.h"
#include "srsran/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsran/phy/upper/channel_processors/pbch_encoder.h"
#include "srsran/support/srsran_test.h"

using namespace srsran;

int main()
{
  std::shared_ptr<crc_calculator_factory> crc_factory = create_crc_calculator_factory_sw("auto");
  TESTASSERT(crc_factory);

  std::shared_ptr<pseudo_random_generator_factory> prg_factory = create_pseudo_random_generator_sw_factory();
  TESTASSERT(prg_factory);

  std::shared_ptr<polar_factory> polar_factory = create_polar_factory_sw();
  TESTASSERT(polar_factory);

  std::shared_ptr<pbch_encoder_factory> encoder_factory =
      create_pbch_encoder_factory_sw(crc_factory, prg_factory, polar_factory);
  TESTASSERT(encoder_factory);

  std::unique_ptr<pbch_encoder> encoder = encoder_factory->create();
  TESTASSERT(encoder);

  for (const test_case_t& test_case : pbch_encoder_test_data) {
    // Encode PBCH message.
    std::array<uint8_t, pbch_encoder::E> encoded_data = {};
    encoder->encode(encoded_data, test_case.pbch_msg);

    // Load output golden data
    const std::vector<uint8_t> testvector_encoded = test_case.encoded.read();

    // Assert encoded data.
    TESTASSERT_EQ(span<const uint8_t>(testvector_encoded), span<const uint8_t>(encoded_data));
  }
  return 0;
}