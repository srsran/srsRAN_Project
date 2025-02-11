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

#include "srsran/fapi/message_builders.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace fapi;

TEST(ul_srs_pdu_builder, valid_basic_parameters_passes)
{
  rnti_t   rnti   = to_rnti(245);
  unsigned handle = 100021;

  ul_srs_pdu         pdu;
  ul_srs_pdu_builder builder(pdu);

  builder.set_basic_parameters(rnti, handle);

  ASSERT_EQ(rnti, pdu.rnti);
  ASSERT_EQ(handle, pdu.handle);
}

TEST(ul_srs_pdu_builder, valid_bwp_parameters_passes)
{
  unsigned           bwp_start = 100;
  unsigned           bwp_size  = 69;
  subcarrier_spacing scs       = subcarrier_spacing::kHz30;
  cyclic_prefix      cp        = cyclic_prefix::EXTENDED;

  ul_srs_pdu         pdu;
  ul_srs_pdu_builder builder(pdu);

  builder.set_bwp_parameters(bwp_size, bwp_start, scs, cp);

  ASSERT_EQ(bwp_size, pdu.bwp_size);
  ASSERT_EQ(bwp_start, pdu.bwp_start);
  ASSERT_EQ(scs, pdu.scs);
  ASSERT_EQ(cp, pdu.cp);
}

TEST(ul_srs_pdu_builder, valid_timing_parameters_passes)
{
  unsigned        t_offset            = 2;
  srs_periodicity t_srs               = srs_periodicity::sl1;
  unsigned        time_start_position = 3;

  ul_srs_pdu         pdu;
  ul_srs_pdu_builder builder(pdu);

  builder.set_timing_params(time_start_position, t_srs, t_offset);

  ASSERT_EQ(time_start_position, pdu.time_start_position);
  ASSERT_EQ(t_srs, pdu.t_srs);
  ASSERT_EQ(t_offset, pdu.t_offset);
}

TEST(ul_srs_pdu_builder, valid_comb_parameters_passes)
{
  unsigned     comb_offset = 2;
  tx_comb_size comb_size   = tx_comb_size::n2;

  ul_srs_pdu         pdu;
  ul_srs_pdu_builder builder(pdu);

  builder.set_comb_params(comb_size, comb_offset);

  ASSERT_EQ(comb_offset, pdu.comb_offset);
  ASSERT_EQ(comb_size, pdu.comb_size);
}

TEST(ul_srs_pdu_builder, valid_frequency_parameters_passes)
{
  unsigned                      frequency_position        = 4;
  unsigned                      frequency_shift           = 3;
  unsigned                      frequency_hopping         = 2;
  srs_group_or_sequence_hopping group_or_sequence_hopping = srs_group_or_sequence_hopping::neither;

  ul_srs_pdu         pdu;
  ul_srs_pdu_builder builder(pdu);

  builder.set_frequency_params(frequency_position, frequency_shift, frequency_hopping, group_or_sequence_hopping);

  ASSERT_EQ(frequency_position, pdu.frequency_position);
  ASSERT_EQ(frequency_shift, pdu.frequency_shift);
  ASSERT_EQ(frequency_hopping, pdu.frequency_hopping);
  ASSERT_EQ(group_or_sequence_hopping, pdu.group_or_sequence_hopping);
}

TEST(ul_srs_pdu_builder, request_normalized_channel_iq_report_passes)
{
  ul_srs_pdu         pdu;
  ul_srs_pdu_builder builder(pdu);

  ASSERT_FALSE(pdu.srs_params_v4.report_type.test(to_value(srs_report_type::normalized_channel_iq_matrix)));

  builder.set_report_params(true, false);

  ASSERT_FALSE(pdu.srs_params_v4.report_type.test(to_value(srs_report_type::positioning)));
  ASSERT_TRUE(pdu.srs_params_v4.report_type.test(to_value(srs_report_type::normalized_channel_iq_matrix)));
}

TEST(ul_srs_pdu_builder, request_positioning_report_passes)
{
  ul_srs_pdu         pdu;
  ul_srs_pdu_builder builder(pdu);

  ASSERT_FALSE(pdu.srs_params_v4.report_type.test(to_value(srs_report_type::positioning)));

  builder.set_report_params(false, true);

  ASSERT_TRUE(pdu.srs_params_v4.report_type.test(to_value(srs_report_type::positioning)));
  ASSERT_FALSE(pdu.srs_params_v4.report_type.test(to_value(srs_report_type::normalized_channel_iq_matrix)));
}

TEST(ul_srs_pdu_builder, request_positioning_and_channel_iq_matrix_reports_passes)
{
  ul_srs_pdu         pdu;
  ul_srs_pdu_builder builder(pdu);

  ASSERT_FALSE(pdu.srs_params_v4.report_type.test(to_value(srs_report_type::positioning)));
  ASSERT_FALSE(pdu.srs_params_v4.report_type.test(to_value(srs_report_type::normalized_channel_iq_matrix)));

  builder.set_report_params(true, true);

  ASSERT_TRUE(pdu.srs_params_v4.report_type.test(to_value(srs_report_type::positioning)));
  ASSERT_TRUE(pdu.srs_params_v4.report_type.test(to_value(srs_report_type::normalized_channel_iq_matrix)));
}

TEST(ul_srs_pdu_builder, valid_srs_parameters_passes)
{
  unsigned          nof_antenna_ports = 4;
  unsigned          nof_symbols       = 3;
  srs_nof_symbols   nof_repetitions   = n1;
  unsigned          config_index      = 2;
  unsigned          sequence_id       = 5;
  unsigned          bandwidth_index   = 6;
  unsigned          cyclic_shift      = 7;
  srs_resource_type resource_type     = srs_resource_type::aperiodic;

  ul_srs_pdu         pdu;
  ul_srs_pdu_builder builder(pdu);

  builder.set_srs_params(nof_antenna_ports,
                         nof_symbols,
                         nof_repetitions,
                         config_index,
                         sequence_id,
                         bandwidth_index,
                         cyclic_shift,
                         resource_type);

  ASSERT_EQ(nof_antenna_ports, pdu.num_ant_ports);
  ASSERT_EQ(nof_symbols, pdu.num_symbols);
  ASSERT_EQ(nof_repetitions, pdu.num_repetitions);
  ASSERT_EQ(config_index, pdu.config_index);
  ASSERT_EQ(sequence_id, pdu.sequence_id);
  ASSERT_EQ(bandwidth_index, pdu.bandwidth_index);
  ASSERT_EQ(cyclic_shift, pdu.cyclic_shift);
  ASSERT_EQ(resource_type, pdu.resource_type);
}
