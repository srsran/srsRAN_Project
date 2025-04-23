/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#pragma once

#include "srsran/phy/upper/channel_processors/pucch/factories.h"
#include "srsran/ran/pucch/pucch_constants.h"
#include <gtest/gtest.h>

namespace srsran {

template <typename PucchProcessorParam, unsigned MaxNofSymbols = MAX_NSYMB_PER_SLOT, unsigned MaxNofRxPorts = 4>
class PucchProcessorTestFixture : public ::testing::TestWithParam<PucchProcessorParam>
{
public:
  static constexpr channel_estimate::channel_estimate_dimensions channel_estimate_dimensions = {
      .nof_prb       = MAX_NOF_PRBS,
      .nof_symbols   = MaxNofSymbols,
      .nof_rx_ports  = MaxNofRxPorts,
      .nof_tx_layers = pucch_constants::MAX_LAYERS,
  };

protected:
  static void SetUpTestSuite()
  {
    std::shared_ptr<low_papr_sequence_generator_factory> lpg_factory = create_low_papr_sequence_generator_sw_factory();
    ASSERT_NE(lpg_factory, nullptr);

    std::shared_ptr<low_papr_sequence_collection_factory> lpc_factory =
        create_low_papr_sequence_collection_sw_factory(lpg_factory);
    ASSERT_NE(lpc_factory, nullptr);

    std::shared_ptr<pseudo_random_generator_factory> prg_factory = create_pseudo_random_generator_sw_factory();
    ASSERT_NE(prg_factory, nullptr);

    std::shared_ptr<dft_processor_factory> dft_factory = create_dft_processor_factory_fftw_slow();
    if (!dft_factory) {
      dft_factory = create_dft_processor_factory_generic();
    }
    ASSERT_NE(dft_factory, nullptr) << "Cannot create DFT factory.";

    std::shared_ptr<time_alignment_estimator_factory> ta_estimator_factory =
        create_time_alignment_estimator_dft_factory(dft_factory);
    ASSERT_NE(ta_estimator_factory, nullptr) << "Cannot create TA estimator factory.";

    // Create channel estimator factory.
    std::shared_ptr<port_channel_estimator_factory> port_chan_estimator_factory =
        create_port_channel_estimator_factory_sw(ta_estimator_factory);
    ASSERT_NE(port_chan_estimator_factory, nullptr) << "Cannot create port channel estimator factory.";

    std::shared_ptr<dmrs_pucch_estimator_factory> estimator_factory =
        create_dmrs_pucch_estimator_factory_sw(prg_factory, lpc_factory, lpg_factory, port_chan_estimator_factory);
    ASSERT_NE(estimator_factory, nullptr) << "Cannot create DM-RS PUCCH estimator factory.";

    // Create factories required by the PUCCH demodulator factory.
    std::shared_ptr<channel_equalizer_factory> equalizer_factory = create_channel_equalizer_generic_factory();
    ASSERT_NE(equalizer_factory, nullptr) << "Cannot create equalizer factory.";

    std::shared_ptr<pucch_detector_factory> detector_factory =
        create_pucch_detector_factory_sw(lpc_factory, prg_factory, equalizer_factory, dft_factory);
    ASSERT_NE(detector_factory, nullptr);

    std::shared_ptr<demodulation_mapper_factory> demod_factory = create_demodulation_mapper_factory();
    ASSERT_NE(demod_factory, nullptr) << "Cannot create channel modulation factory.";

    std::shared_ptr<transform_precoder_factory> precoding_factory =
        create_dft_transform_precoder_factory(dft_factory, pucch_constants::FORMAT3_MAX_NPRB + 1);
    ASSERT_NE(precoding_factory, nullptr) << "Cannot create transform precoder factory.";

    // Create PUCCH demodulator factory.
    std::shared_ptr<pucch_demodulator_factory> pucch_demod_factory =
        create_pucch_demodulator_factory_sw(equalizer_factory, demod_factory, prg_factory, precoding_factory);
    ASSERT_NE(pucch_demod_factory, nullptr) << "Cannot create PUCCH demodulator factory.";

    // Create short block detector factory.
    std::shared_ptr<short_block_detector_factory> short_block_det_factory = create_short_block_detector_factory_sw();
    ASSERT_NE(short_block_det_factory, nullptr) << "Cannot create short block detector factory.";

    // Create polar decoder factory.
    std::shared_ptr<polar_factory> polar_dec_factory = create_polar_factory_sw();
    ASSERT_NE(polar_dec_factory, nullptr) << "Invalid polar decoder factory.";

    // Create CRC calculator factory.
    std::shared_ptr<crc_calculator_factory> crc_calc_factory = create_crc_calculator_factory_sw("auto");
    ASSERT_NE(crc_calc_factory, nullptr) << "Invalid CRC calculator factory.";

    // Create UCI decoder factory.
    std::shared_ptr<uci_decoder_factory> uci_dec_factory =
        create_uci_decoder_factory_generic(short_block_det_factory, polar_dec_factory, crc_calc_factory);
    ASSERT_NE(uci_dec_factory, nullptr) << "Cannot create UCI decoder factory.";

    std::shared_ptr<pucch_processor_factory> factory = create_pucch_processor_factory_sw(
        estimator_factory, detector_factory, pucch_demod_factory, uci_dec_factory, channel_estimate_dimensions);
    ASSERT_NE(factory, nullptr);

    processor = factory->create();
    ASSERT_NE(processor, nullptr) << "Could not create PUCCH processor.";
    validator = factory->create_validator();
    ASSERT_NE(validator, nullptr) << "Could not create PUCCH validator.";
  }

  static void TearDownTestSuite()
  {
    processor.reset();
    validator.reset();
  }

  void SetUp() override
  {
    ASSERT_NE(processor, nullptr) << "Could not create PUCCH processor.";
    ASSERT_NE(validator, nullptr) << "Could not create PUCCH validator.";
  }

  // PUCCH processor.
  static std::unique_ptr<pucch_processor> processor;
  // PUCCH processor validator.
  static std::unique_ptr<pucch_pdu_validator> validator;
};

} // namespace srsran
