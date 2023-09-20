/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

/// \file
/// \brief UCI decoder unit test.
///
/// Detects short messages from sequences of log-likelihood ratios. Both the LLR sequences and the corresponding
/// messages (for comparison purposes) are provided as test vectors.

#include "uci_decoder_test_data.h"
#include "srsran/phy/upper/channel_coding/channel_coding_factories.h"
#include "srsran/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsran/support/srsran_test.h"

/// \cond
using namespace srsran;

int main()
{
  std::shared_ptr<short_block_detector_factory> short_block_detector_factory = create_short_block_detector_factory_sw();
  TESTASSERT(short_block_detector_factory);

  std::shared_ptr<polar_factory> polar_decoder_factory = create_polar_factory_sw();
  TESTASSERT(polar_decoder_factory);

  std::shared_ptr<crc_calculator_factory> crc_calc_factory = create_crc_calculator_factory_sw("auto");
  TESTASSERT(crc_calc_factory);

  std::shared_ptr<uci_decoder_factory> uci_decoder_factory =
      create_uci_decoder_factory_sw(short_block_detector_factory, polar_decoder_factory, crc_calc_factory);
  TESTASSERT(uci_decoder_factory);

  std::unique_ptr<uci_decoder> decoder = uci_decoder_factory->create();
  TESTASSERT(decoder);

  for (const auto& test_data : uci_decoder_test_data) {
    uci_decoder::configuration dec_cfg = {};
    dec_cfg.modulation                 = test_data.config.modulation;

    const std::vector<uint8_t> message = test_data.message.read();
    TESTASSERT_EQ(message.size(), test_data.message_length, "Error reading message.");
    std::vector<log_likelihood_ratio> llr = test_data.llr.read();
    TESTASSERT_EQ(llr.size(), test_data.llr_length, "Error reading LLRs.");

    std::vector<uint8_t> message_test(message.size());
    uci_status           status = decoder->decode(message_test, llr, dec_cfg);
    TESTASSERT_NEQ(status, uci_status::unknown, "Decoder in an unknown status.");
    TESTASSERT_NEQ(status, uci_status::invalid, "Invalid detection.");
    TESTASSERT_EQ(span<const uint8_t>(message_test), span<const uint8_t>(message), "Detection went wrong.");
  }
}
/// \endcond
