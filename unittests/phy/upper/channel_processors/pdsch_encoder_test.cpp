/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pdsch_encoder_test_data.h"
#include "srsgnb/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsgnb/support/srsgnb_test.h"

using namespace srsgnb;
using namespace srsgnb::ldpc;

int main()
{
  std::shared_ptr<crc_calculator_factory> crc_calc_factory = create_crc_calculator_factory_sw();
  TESTASSERT(crc_calc_factory);

  std::shared_ptr<ldpc_encoder_factory> ldpc_encoder_factory = create_ldpc_encoder_factory_sw("generic");
  TESTASSERT(ldpc_encoder_factory);

  std::shared_ptr<ldpc_rate_matcher_factory> ldpc_rate_matcher_factory = create_ldpc_rate_matcher_factory_sw();
  TESTASSERT(ldpc_rate_matcher_factory);

  std::shared_ptr<ldpc_segmenter_tx_factory> segmenter_factory = create_ldpc_segmenter_tx_factory_sw(crc_calc_factory);
  TESTASSERT(segmenter_factory);

  pdsch_encoder_factory_sw_configuration encoder_factory_config;
  encoder_factory_config.encoder_factory                 = ldpc_encoder_factory;
  encoder_factory_config.rate_matcher_factory            = ldpc_rate_matcher_factory;
  encoder_factory_config.segmenter_factory               = segmenter_factory;
  std::shared_ptr<pdsch_encoder_factory> encoder_factory = create_pdsch_encoder_factory_sw(encoder_factory_config);
  TESTASSERT(encoder_factory);

  std::shared_ptr<pdsch_encoder> pdsch_encoder = encoder_factory->create();
  TESTASSERT(pdsch_encoder);

  for (const test_case_t& test_case : pdsch_encoder_test_data) {
    // Load the TB.
    std::vector<uint8_t> transport_block = test_case.transport_block.read();

    // Load the golden data.
    std::vector<uint8_t> expected_codeword = test_case.encoded.read();

    // Assert codeword length.
    constexpr unsigned MAX_CW_LENGTH = 40000;
    unsigned           cw_length =
        test_case.config.nof_ch_symbols * static_cast<unsigned>(test_case.config.mod) * test_case.config.nof_layers;
    TESTASSERT_EQ(cw_length, expected_codeword.size(), "Wrong codeword length.");
    static_vector<uint8_t, MAX_CW_LENGTH> codeword(cw_length);

    // Encode the TB.
    pdsch_encoder->encode(codeword, transport_block, test_case.config);

    // Assert encoded data.
    TESTASSERT(std::equal(codeword.begin(), codeword.end(), expected_codeword.begin()), "Encoding went wrong.");
  }
}
