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

#include "helpers.h"
#include "srsran/fapi_adaptor/mac/messages/srs.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace fapi_adaptor;
using namespace unittests;

TEST(mac_to_fapi_srs_pdu_test, valid_srs_pdu_should_pass)
{
  const srs_info_helper& pdu_test = build_valid_srs_pdu();
  const srs_info&        mac_pdu  = pdu_test.pdu;
  fapi::ul_srs_pdu       fapi_pdu;

  convert_srs_mac_to_fapi(fapi_pdu, mac_pdu);

  ASSERT_EQ(mac_pdu.crnti, fapi_pdu.rnti);

  // BWP.
  ASSERT_EQ(mac_pdu.bwp_cfg->cp, fapi_pdu.cp);
  ASSERT_EQ(mac_pdu.bwp_cfg->scs, fapi_pdu.scs);
  ASSERT_EQ(mac_pdu.bwp_cfg->crbs.start(), fapi_pdu.bwp_start);
  ASSERT_EQ(mac_pdu.bwp_cfg->crbs.length(), fapi_pdu.bwp_size);

  ASSERT_EQ(mac_pdu.nof_antenna_ports, fapi_pdu.num_ant_ports);
  ASSERT_EQ(mac_pdu.symbols.length(), fapi_pdu.num_symbols);
  ASSERT_EQ(0, fapi_pdu.time_start_position);
  ASSERT_EQ(mac_pdu.config_index, fapi_pdu.config_index);
  ASSERT_EQ(mac_pdu.sequence_id, fapi_pdu.sequence_id);
  ASSERT_EQ(mac_pdu.bw_index, fapi_pdu.bandwidth_index);
  ASSERT_EQ(mac_pdu.tx_comb, fapi_pdu.comb_size);
  ASSERT_EQ(mac_pdu.comb_offset, fapi_pdu.comb_offset);
  ASSERT_EQ(mac_pdu.cyclic_shift, fapi_pdu.cyclic_shift);
  ASSERT_EQ(mac_pdu.freq_position, fapi_pdu.frequency_position);
  ASSERT_EQ(mac_pdu.freq_shift, fapi_pdu.frequency_shift);
  ASSERT_EQ(mac_pdu.freq_hopping, fapi_pdu.frequency_hopping);
  ASSERT_EQ(mac_pdu.group_or_seq_hopping, fapi_pdu.group_or_sequence_hopping);
  ASSERT_EQ(mac_pdu.resource_type, fapi_pdu.resource_type);
  ASSERT_EQ(mac_pdu.t_srs_period, fapi_pdu.t_srs);
  ASSERT_EQ(mac_pdu.t_offset, fapi_pdu.t_offset);
  ASSERT_EQ(mac_pdu.normalized_channel_iq_matrix_requested,
            fapi_pdu.srs_params_v4.report_type.test(to_value(fapi::srs_report_type::normalized_channel_iq_matrix)));
  ASSERT_EQ(mac_pdu.positioning_report_requested,
            fapi_pdu.srs_params_v4.report_type.test(to_value(fapi::srs_report_type::positioning)));
}
