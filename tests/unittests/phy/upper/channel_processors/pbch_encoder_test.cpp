/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
    for (unsigned i = 0; i != pbch_encoder::E; ++i) {
      TESTASSERT_EQ(encoded_data[i], testvector_encoded[i]);
    }
  }
  return 0;
}