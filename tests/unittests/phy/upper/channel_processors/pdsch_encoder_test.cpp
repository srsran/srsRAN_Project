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

#include "pdsch_encoder_test_data.h"
#include "srsran/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsran/phy/upper/tx_buffer_pool.h"
#include "srsran/phy/upper/unique_tx_buffer.h"
#include "srsran/ran/pdsch/pdsch_constants.h"
#include "srsran/support/srsran_test.h"

using namespace srsran;
using namespace srsran::ldpc;

int main()
{
  std::shared_ptr<crc_calculator_factory> crc_calc_factory = create_crc_calculator_factory_sw("auto");
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

  tx_buffer_pool_config buffer_pool_config;
  buffer_pool_config.max_codeblock_size          = ldpc::MAX_CODEBLOCK_SIZE;
  buffer_pool_config.nof_buffers                 = 1;
  buffer_pool_config.nof_codeblocks              = pdsch_constants::CODEWORD_MAX_SIZE.value() / ldpc::MAX_MESSAGE_SIZE;
  buffer_pool_config.expire_timeout_slots        = 0;
  buffer_pool_config.external_soft_bits          = false;
  std::shared_ptr<tx_buffer_pool> rm_buffer_pool = create_tx_buffer_pool(buffer_pool_config);

  trx_buffer_identifier buffer_id(0, 0);

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

    unsigned nof_codeblocks =
        ldpc::compute_nof_codeblocks(units::bits(transport_block.size() * 8), test_case.config.base_graph);

    unique_tx_buffer rm_buffer = rm_buffer_pool->reserve(slot_point(), buffer_id, nof_codeblocks);

    pdsch_encoder::configuration config;
    config.new_data       = true;
    config.base_graph     = test_case.config.base_graph;
    config.rv             = test_case.config.rv;
    config.mod            = test_case.config.mod;
    config.Nref           = test_case.config.Nref;
    config.nof_layers     = test_case.config.nof_layers;
    config.nof_ch_symbols = test_case.config.nof_ch_symbols;

    // Encode the TB.
    pdsch_encoder->encode(codeword, rm_buffer.get(), transport_block, config);

    // Assert encoded data.
    TESTASSERT_EQ(span<const uint8_t>(codeword), span<const uint8_t>(expected_codeword));

    // Repeat test reusing the buffer.
    config.new_data = false;
    pdsch_encoder->encode(codeword, rm_buffer.get(), transport_block, config);
    TESTASSERT_EQ(span<const uint8_t>(codeword), span<const uint8_t>(expected_codeword));
  }
}
