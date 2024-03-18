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

#include "srsran/fapi/message_builders.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace fapi;

TEST(dl_ssb_pdu_builder, valid_basic_parameters_passes)
{
  dl_ssb_pdu         pdu;
  dl_ssb_pdu_builder builder(pdu);

  pci_t                 pci               = 106;
  beta_pss_profile_type pss_profile       = beta_pss_profile_type::dB_0;
  unsigned              block_index       = 3;
  unsigned              subcarrier_offset = 2;
  unsigned              offset_pointA     = 39;

  builder.set_basic_parameters(pci, pss_profile, block_index, subcarrier_offset, offset_pointA);

  ASSERT_EQ(pci, pdu.phys_cell_id);
  ASSERT_EQ(pss_profile, pdu.beta_pss_profile_nr);
  ASSERT_EQ(block_index, pdu.ssb_block_index);
  ASSERT_EQ(subcarrier_offset, pdu.ssb_subcarrier_offset);
  ASSERT_EQ(offset_pointA, pdu.ssb_offset_pointA.to_uint());
}

TEST(dl_ssb_pdu_builder, valid_maintenance_v3_parameters_passes)
{
  dl_ssb_pdu         pdu;
  dl_ssb_pdu_builder builder(pdu);

  ssb_pattern_case   ssb_case = ssb_pattern_case::A;
  subcarrier_spacing scs      = subcarrier_spacing::kHz15;
  unsigned           Lmax     = 8;

  builder.set_maintenance_v3_basic_parameters(ssb_case, scs, Lmax);

  const dl_ssb_maintenance_v3& v3 = pdu.ssb_maintenance_v3;
  ASSERT_EQ(ssb_case, v3.case_type);
  ASSERT_EQ(scs, v3.scs);
  ASSERT_EQ(Lmax, v3.L_max);
}

TEST(dl_ssb_pdu_builder, valid_bch_payload_passes)
{
  dl_ssb_pdu         pdu;
  dl_ssb_pdu_builder builder(pdu);

  unsigned payload = 5453423;

  builder.set_bch_payload_mac_full(payload);

  ASSERT_EQ(bch_payload_type::mac_full, pdu.bch_payload_flag);
  ASSERT_EQ(payload, pdu.bch_payload.bch_payload);
}

TEST(dl_ssb_pdu_builder, valid_bch_payload_mixed_passes)
{
  dl_ssb_pdu         pdu;
  dl_ssb_pdu_builder builder(pdu);

  unsigned payload = 15453423;

  builder.set_bch_payload_phy_timing_info(payload);

  ASSERT_EQ(bch_payload_type::phy_timing_info, pdu.bch_payload_flag);
  ASSERT_EQ(payload, pdu.bch_payload.bch_payload);
}

TEST(dl_ssb_pdu_builder, valid_bch_payload_phy_passes)
{
  dl_ssb_pdu         pdu;
  dl_ssb_pdu_builder builder(pdu);

  dmrs_typeA_position dmrs_typeA_pos         = dmrs_typeA_position::pos2;
  unsigned            pdcch_sib1             = 134;
  bool                barred                 = true;
  bool                intra_freq_reselection = false;

  builder.set_bch_payload_phy_full(dmrs_typeA_pos, pdcch_sib1, barred, intra_freq_reselection);

  ASSERT_EQ(bch_payload_type::phy_full, pdu.bch_payload_flag);
  const dl_ssb_phy_mib_pdu& mib = pdu.bch_payload.phy_mib_pdu;
  ASSERT_EQ((dmrs_typeA_pos == dmrs_typeA_position::pos2) ? dmrs_typeA_pos::pos2 : dmrs_typeA_pos::pos3,
            mib.dmrs_typeA_position);
  ASSERT_EQ(pdcch_sib1, mib.pdcch_config_sib1);
  ASSERT_EQ(barred, mib.cell_barred);
  ASSERT_EQ(intra_freq_reselection, mib.intrafreq_reselection);
}
