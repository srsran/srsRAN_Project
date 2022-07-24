/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/fapi/message_builders.h"
#include "srsgnb/support/srsgnb_test.h"
#include <random>

using namespace srsgnb;
using namespace fapi;

static std::mt19937 gen(0);

static void test_basic_params()
{
  dl_ssb_pdu         pdu;
  dl_ssb_pdu_builder builder(pdu);

  pci_t                 pci               = 106;
  beta_pss_profile_type pss_profile       = beta_pss_profile_type::dB_0;
  unsigned              block_index       = 3;
  unsigned              subcarrier_offset = 2;
  unsigned              offset_pointA     = 39;

  builder.set_basic_parameters(pci, pss_profile, block_index, subcarrier_offset, offset_pointA);

  TESTASSERT_EQ(pci, pdu.phys_cell_id);
  TESTASSERT_EQ(pss_profile, pdu.beta_pss_profile_nr);
  TESTASSERT_EQ(block_index, pdu.ssb_block_index);
  TESTASSERT_EQ(subcarrier_offset, pdu.ssb_subcarrier_offset);
  TESTASSERT_EQ(offset_pointA, pdu.ssb_offset_pointA);
}

static void test_maintenance_v3_basic_params()
{
  dl_ssb_pdu         pdu;
  dl_ssb_pdu_builder builder(pdu);

  ssb_pattern_case   ssb_case = ssb_pattern_case::A;
  subcarrier_spacing scs      = subcarrier_spacing::kHz15;
  unsigned           Lmax     = 8;

  builder.set_maintenance_v3_basic_parameters(ssb_case, scs, Lmax);

  const dl_ssb_maintenance_v3& v3 = pdu.ssb_maintenance_v3;
  TESTASSERT_EQ(ssb_case, v3.case_type);
  TESTASSERT_EQ(scs, v3.scs);
  TESTASSERT_EQ(Lmax, v3.lmax);
}

static void test_maintenance_v3_tx_power_params()
{
  for (bool enable : {true, false}) {
    dl_ssb_pdu         pdu;
    dl_ssb_pdu_builder builder(pdu);

    optional<float> pss_profile_sss;
    optional<float> ss_power_block;
    if (enable) {
      pss_profile_sss.emplace(18.307);
      ss_power_block.emplace(-8.607);
    }

    builder.set_maintenance_v3_tx_power_info(ss_power_block, pss_profile_sss);

    const dl_ssb_maintenance_v3& v3 = pdu.ssb_maintenance_v3;

    TESTASSERT_EQ(enable ? 18307 : -32768, v3.beta_pss_profile_sss);
    TESTASSERT_EQ(enable ? -860 : -32768, v3.ss_pbch_block_power_scaling);
  }
}

static void test_bch_payload_mac()
{
  dl_ssb_pdu         pdu;
  dl_ssb_pdu_builder builder(pdu);

  unsigned payload = 5453423;

  builder.set_bch_payload_mac_full(payload);

  TESTASSERT_EQ(bch_payload_type::mac_full, pdu.bch_payload_flag);
  TESTASSERT_EQ(payload, pdu.bch_payload.bch_payload);
}

static void test_bch_payload_mixed()
{
  dl_ssb_pdu         pdu;
  dl_ssb_pdu_builder builder(pdu);

  unsigned payload = 15453423;

  builder.set_bch_payload_phy_timing_info(payload);

  TESTASSERT_EQ(bch_payload_type::phy_timing_info, pdu.bch_payload_flag);
  TESTASSERT_EQ(payload, pdu.bch_payload.bch_payload);
}

static void test_bch_payload_phy()
{
  dl_ssb_pdu         pdu;
  dl_ssb_pdu_builder builder(pdu);

  unsigned dmrs_typeA_position    = 1;
  unsigned pdcch_sib1             = 134;
  bool     barred                 = true;
  bool     intra_freq_reselection = false;

  builder.set_bch_payload_phy_full(dmrs_typeA_position, pdcch_sib1, barred, intra_freq_reselection);

  TESTASSERT_EQ(bch_payload_type::phy_full, pdu.bch_payload_flag);
  const dl_ssb_phy_mib_pdu& mib = pdu.bch_payload.phy_mib_pdu;
  TESTASSERT_EQ(dmrs_typeA_position, mib.dmrs_typeA_position);
  TESTASSERT_EQ(pdcch_sib1, mib.pdcch_config_sib1);
  TESTASSERT_EQ(barred, mib.cell_barred);
  TESTASSERT_EQ(intra_freq_reselection, mib.intrafreq_reselection);
}

int main()
{
  test_basic_params();
  test_maintenance_v3_basic_params();
  test_maintenance_v3_tx_power_params();
  test_bch_payload_mac();
  test_bch_payload_mixed();
  test_bch_payload_phy();

  return 0;
}
