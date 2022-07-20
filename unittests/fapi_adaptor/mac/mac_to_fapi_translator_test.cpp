/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../../lib/fapi_adaptor/mac/mac_to_fapi_translator.h"
#include "messages/helpers.h"
#include "srsgnb/fapi/messages.h"
#include "srsgnb/support/srsgnb_test.h"

using namespace srsgnb;
using namespace fapi;
using namespace fapi_adaptor;
using namespace unittests;

namespace {

/// Spy implementation of a slot message gateway.
class slot_message_gateway_spy : public slot_message_gateway
{
  bool                   has_dl_tti_request_method_been_called = false;
  dl_tti_request_message dl_tti_msg;

public:
  bool has_dl_tti_request_method_called() const { return has_dl_tti_request_method_been_called; }
  const dl_tti_request_message& dl_tti_request_msg() const { return dl_tti_msg; }

  void dl_tti_request(const dl_tti_request_message& msg) override
  {
    has_dl_tti_request_method_been_called = true;
    dl_tti_msg                            = msg;
  }

  void ul_tti_request(const ul_tti_request_message& msg) override {}

  void ul_dci_request(const ul_dci_request_message& msg) override {}

  void tx_data_request(const tx_data_request_message& msg) override {}
};

} // namespace

static void test_sched_result_ok()
{
  slot_message_gateway_spy gateway_spy;
  mac_to_fapi_translator   translator(gateway_spy);

  TESTASSERT(!gateway_spy.has_dl_tti_request_method_called());

  const mac_dl_sched_result& result = build_valid_mac_dl_sched_result();
  translator.on_new_downlink_scheduler_results(result);

  TESTASSERT(gateway_spy.has_dl_tti_request_method_called());
  const dl_tti_request_message& msg = gateway_spy.dl_tti_request_msg();
  TESTASSERT_EQ(msg.pdus.size(), 5U);
  TESTASSERT_EQ((msg.pdus.begin() + 1)->pdu_type, dl_pdu_type::PDCCH);
  TESTASSERT_EQ(msg.pdus.back().pdu_type, dl_pdu_type::PDSCH);
  TESTASSERT_EQ(msg.pdus.front().pdu_type, dl_pdu_type::PDCCH);
  TESTASSERT_EQ(msg.pdus.front().pdcch_pdu.dl_dci.size(), 3U);
  TESTASSERT_EQ((msg.pdus.begin() + 1)->pdcch_pdu.dl_dci.size(), 1U);
  TESTASSERT_EQ((msg.pdus.end() - 2)->pdu_type, dl_pdu_type::SSB);
  TESTASSERT_EQ((msg.pdus.end() - 3)->pdu_type, dl_pdu_type::SSB);

  const srsgnb::dl_ssb_pdu& pdu      = result.ssb_pdu.front();
  const fapi::dl_ssb_pdu&   fapi_pdu = (msg.pdus.end() - 3)->ssb_pdu;
  TESTASSERT_EQ(pdu.pci, fapi_pdu.phys_cell_id);
  TESTASSERT_EQ(static_cast<unsigned>(pdu.pss_to_sss_epre), static_cast<unsigned>(fapi_pdu.beta_pss_profile_nr));
  TESTASSERT_EQ(pdu.ssb_index, fapi_pdu.ssb_block_index);
  TESTASSERT_EQ(pdu.subcarrier_offset.to_uint(), fapi_pdu.ssb_subcarrier_offset);
  TESTASSERT_EQ(pdu.offset_to_pointA.to_uint(), fapi_pdu.ssb_offset_pointA);

  // Maintenance v3 parameters.
  TESTASSERT_EQ(static_cast<unsigned>(pdu.ssb_case), static_cast<unsigned>(fapi_pdu.ssb_maintenance_v3.case_type));
  TESTASSERT_EQ(pdu.L_max, fapi_pdu.ssb_maintenance_v3.lmax);
  TESTASSERT_EQ(static_cast<unsigned>(pdu.scs), static_cast<unsigned>(fapi_pdu.ssb_maintenance_v3.scs));
  TESTASSERT_EQ(-32768, fapi_pdu.ssb_maintenance_v3.beta_pss_profile_sss);
  TESTASSERT_EQ(-32768, fapi_pdu.ssb_maintenance_v3.ss_pbch_block_power_scaling);

  // MIB.
  TESTASSERT_EQ(bch_payload_type::phy_full, fapi_pdu.bch_payload_flag);
  TESTASSERT_EQ(pdu.mib_data.pdcch_config_sib1, fapi_pdu.bch_payload.phy_mib_pdu.pdcch_config_sib1);
  uint8_t dmrs_typeA_pos = pdu.mib_data.dmrs_typeA_pos == dmrs_typeA_position::pos2 ? 0 : 1;
  TESTASSERT_EQ(dmrs_typeA_pos, fapi_pdu.bch_payload.phy_mib_pdu.dmrs_typeA_position);
  TESTASSERT_EQ(pdu.mib_data.cell_barred, fapi_pdu.bch_payload.phy_mib_pdu.cell_barred);
  TESTASSERT_EQ(pdu.mib_data.intra_freq_reselection, fapi_pdu.bch_payload.phy_mib_pdu.intrafreq_reselection);
}

int main()
{
  test_sched_result_ok();
}
