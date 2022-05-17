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
#include "srsgnb/phy/upper/channel_processors/pdsch_encoder.h"
#include "srsgnb/support/srsgnb_test.h"
#include <iostream>

using namespace srsgnb;
using namespace srsgnb::ldpc;

int main()
{
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
    std::unique_ptr<pdsch_encoder> pdsch_encoder = create_pdsch_encoder();
    pdsch_encoder->encode(codeword, transport_block, test_case.config);

    // Assert encoded data.
    TESTASSERT(std::equal(codeword.begin(), codeword.end(), expected_codeword.begin()), "Encoding went wrong.");
  }
}
