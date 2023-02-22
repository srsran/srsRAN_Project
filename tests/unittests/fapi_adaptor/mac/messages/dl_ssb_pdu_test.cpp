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

#include "helpers.h"
#include "srsran/fapi_adaptor/mac/messages/ssb.h"
#include "srsran/mac/mac_cell_result.h"
#include "srsran/support/srsran_test.h"

using namespace srsran;
using namespace fapi_adaptor;
using namespace unittests;

static void test_conversion_ok()
{
  srsran::dl_ssb_pdu pdu = build_valid_dl_ssb_pdu();

  fapi::dl_ssb_pdu fapi_pdu;
  convert_ssb_mac_to_fapi(fapi_pdu, pdu);

  TESTASSERT_EQ(pdu.pci, fapi_pdu.phys_cell_id);
  TESTASSERT_EQ(static_cast<unsigned>(pdu.pss_to_sss_epre), static_cast<unsigned>(fapi_pdu.beta_pss_profile_nr));
  TESTASSERT_EQ(pdu.ssb_index, fapi_pdu.ssb_block_index);
  TESTASSERT_EQ(pdu.subcarrier_offset.to_uint(), fapi_pdu.ssb_subcarrier_offset);
  TESTASSERT_EQ(pdu.offset_to_pointA.to_uint(), fapi_pdu.ssb_offset_pointA.to_uint());

  // Maintenance v3 parameters.
  TESTASSERT_EQ(static_cast<unsigned>(pdu.ssb_case), static_cast<unsigned>(fapi_pdu.ssb_maintenance_v3.case_type));
  TESTASSERT_EQ(pdu.L_max, fapi_pdu.ssb_maintenance_v3.L_max);
  TESTASSERT_EQ(static_cast<unsigned>(pdu.scs), static_cast<unsigned>(fapi_pdu.ssb_maintenance_v3.scs));
  TESTASSERT_EQ(-32768, fapi_pdu.ssb_maintenance_v3.beta_pss_profile_sss);
  TESTASSERT_EQ(-32768, fapi_pdu.ssb_maintenance_v3.ss_pbch_block_power_scaling);

  // MIB.
  TESTASSERT_EQ(fapi::bch_payload_type::phy_full, fapi_pdu.bch_payload_flag);
  TESTASSERT_EQ(pdu.mib_data.pdcch_config_sib1, fapi_pdu.bch_payload.phy_mib_pdu.pdcch_config_sib1);
  auto dmrs_typeA_pos = pdu.mib_data.dmrs_typeA_pos == dmrs_typeA_position::pos2 ? fapi::dmrs_typeA_pos::pos2
                                                                                 : fapi::dmrs_typeA_pos::pos3;
  TESTASSERT_EQ(dmrs_typeA_pos, fapi_pdu.bch_payload.phy_mib_pdu.dmrs_typeA_position);
  TESTASSERT_EQ(pdu.mib_data.cell_barred, fapi_pdu.bch_payload.phy_mib_pdu.cell_barred);
  TESTASSERT_EQ(pdu.mib_data.intra_freq_reselection, fapi_pdu.bch_payload.phy_mib_pdu.intrafreq_reselection);
}

int main()
{
  test_conversion_ok();
}
