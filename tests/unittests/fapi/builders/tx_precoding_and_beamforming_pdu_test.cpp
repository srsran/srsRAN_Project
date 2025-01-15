/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/fapi/message_builders.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace fapi;

TEST(tx_precoding_and_beamforming_pdu_builder, creating_builder_sets_trp_scheme_to_zero)
{
  tx_precoding_and_beamforming_pdu pdu;
  ASSERT_FALSE(pdu.trp_scheme == 0);

  tx_precoding_and_beamforming_pdu_builder builder(pdu);

  ASSERT_TRUE(pdu.trp_scheme == 0);
}

TEST(tx_precoding_and_beamforming_pdu_builder, valid_basic_parameters_passes)
{
  tx_precoding_and_beamforming_pdu         pdu;
  tx_precoding_and_beamforming_pdu_builder builder(pdu);

  unsigned dig_bf_interfaces = 2;
  unsigned prg_size          = 4;

  builder.set_basic_parameters(prg_size, dig_bf_interfaces);

  ASSERT_EQ(prg_size, pdu.prg_size);
  ASSERT_EQ(dig_bf_interfaces, pdu.dig_bf_interfaces);
}

TEST(tx_precoding_and_beamforming_pdu_builder, add_prg_passes)
{
  tx_precoding_and_beamforming_pdu         pdu;
  tx_precoding_and_beamforming_pdu_builder builder(pdu);

  unsigned prg_size          = 8;
  unsigned dig_bf_interfaces = 2;
  unsigned pm_index          = 4;
  static_vector<uint16_t, tx_precoding_and_beamforming_pdu::MAX_NUM_SUPPORTED_DIGITAL_BEAMFORMING_INTERFACES>
      beam_indexes = {8, 9};

  ASSERT_TRUE(pdu.prgs.empty());

  builder.set_basic_parameters(prg_size, dig_bf_interfaces);
  builder.add_prg(pm_index, {beam_indexes});

  ASSERT_EQ(pdu.prgs.size(), 1);
  const auto& prg = pdu.prgs.front();
  ASSERT_EQ(pm_index, prg.pm_index);
  ASSERT_EQ(beam_indexes, prg.beam_index);
}
