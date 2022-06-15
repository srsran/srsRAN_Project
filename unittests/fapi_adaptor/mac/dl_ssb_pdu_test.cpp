/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/fapi_adaptor/mac/messages/ssb.h"
#include "srsgnb/mac/mac_cell_result.h"
#include "srsgnb/support/srsgnb_test.h"

using namespace srsgnb;
using namespace fapi;
using namespace fapi_adaptor;

static void test_conversion_ok()
{
  srsgnb::dl_ssb_pdu pdu;
  pdu.pci                             = 123;
  pdu.beta_pss_profile_nr             = srsgnb::beta_pss_profile_nr_type::dB_0;
  pdu.ssb_index                       = 4;
  pdu.ssb_subcarrier_offset           = 14;
  pdu.offset_to_point_A               = 500;
  pdu.ssb_case                        = ssb_pattern_case::A;
  pdu.L_max                           = 4;
  pdu.scs                             = subcarrier_spacing::kHz30;
  pdu.mib_data.dmrs_typeA_position    = 0;
  pdu.mib_data.pdcch_config_sib1      = 2;
  pdu.mib_data.cell_barred            = true;
  pdu.mib_data.intra_freq_reselection = false;

  fapi::dl_ssb_pdu fapi_pdu;
  convert_ssb_mac_to_fapi(fapi_pdu, pdu);

  TESTASSERT_EQ(pdu.pci, fapi_pdu.phys_cell_id);
  TESTASSERT_EQ(static_cast<unsigned>(pdu.beta_pss_profile_nr), static_cast<unsigned>(fapi_pdu.beta_pss_profile_nr));
  TESTASSERT_EQ(pdu.ssb_index, fapi_pdu.ssb_block_index);
  TESTASSERT_EQ(pdu.ssb_subcarrier_offset, fapi_pdu.ssb_subcarrier_offset);
  TESTASSERT_EQ(pdu.offset_to_point_A, fapi_pdu.ssb_offset_pointA);

  // Maintenance v3 parameters.
  TESTASSERT_EQ(static_cast<unsigned>(pdu.ssb_case), static_cast<unsigned>(fapi_pdu.ssb_maintenance_v3.case_type));
  TESTASSERT_EQ(pdu.L_max, fapi_pdu.ssb_maintenance_v3.lmax);
  TESTASSERT_EQ(static_cast<unsigned>(pdu.scs), static_cast<unsigned>(fapi_pdu.ssb_maintenance_v3.scs));
  TESTASSERT_EQ(-32768, fapi_pdu.ssb_maintenance_v3.beta_pss_profile_sss);
  TESTASSERT_EQ(-32768, fapi_pdu.ssb_maintenance_v3.ss_pbch_block_power_scaling);

  // MIB.
  TESTASSERT_EQ(bch_payload_type::phy_full, fapi_pdu.bch_payload_flag);
  TESTASSERT_EQ(pdu.mib_data.pdcch_config_sib1, fapi_pdu.bch_payload.phy_mib_pdu.pdcch_config_sib1);
  TESTASSERT_EQ(pdu.mib_data.dmrs_typeA_position, fapi_pdu.bch_payload.phy_mib_pdu.dmrs_typeA_position);
  TESTASSERT_EQ(pdu.mib_data.cell_barred, fapi_pdu.bch_payload.phy_mib_pdu.cell_barred);
  TESTASSERT_EQ(pdu.mib_data.intra_freq_reselection, fapi_pdu.bch_payload.phy_mib_pdu.intrafreq_reselection);
}

int main()
{
  test_conversion_ok();
}
