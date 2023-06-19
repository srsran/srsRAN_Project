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

#include "../../../lib/fapi_adaptor/mac/mac_to_fapi_translator.h"
#include "messages/helpers.h"
#include "srsran/fapi/messages.h"
#include "srsran/fapi/slot_last_message_notifier.h"
#include "srsran/fapi/slot_message_gateway.h"
#include "srsran/fapi_adaptor/precoding_matrix_table_generator.h"
#include "srsran/support/srsran_test.h"

using namespace srsran;
using namespace fapi_adaptor;
using namespace unittests;

namespace {

/// Spy implementation of a slot message gateway.
class slot_message_gateway_spy : public fapi::slot_message_gateway
{
  bool                         has_dl_tti_request_method_been_called = false;
  fapi::dl_tti_request_message dl_tti_msg;

public:
  bool has_dl_tti_request_method_called() const { return has_dl_tti_request_method_been_called; }
  const fapi::dl_tti_request_message& dl_tti_request_msg() const { return dl_tti_msg; }

  void dl_tti_request(const fapi::dl_tti_request_message& msg) override
  {
    has_dl_tti_request_method_been_called = true;
    dl_tti_msg                            = msg;
  }

  void ul_tti_request(const fapi::ul_tti_request_message& msg) override {}

  void ul_dci_request(const fapi::ul_dci_request_message& msg) override {}

  void tx_data_request(const fapi::tx_data_request_message& msg) override {}
};

class slot_last_message_notifier_dummy : public fapi::slot_last_message_notifier
{
public:
  void on_last_message(slot_point slot) override {}
};

} // namespace

static void test_valid_dl_sched_results_generate_correct_dl_tti_request()
{
  slot_message_gateway_spy         gateway_spy;
  slot_last_message_notifier_dummy dummy_notifier;
  unsigned                         nof_prbs = 51U;
  auto                             pm_tools = generate_precoding_matrix_tables(1);
  mac_to_fapi_translator           translator(
      srslog::fetch_basic_logger("FAPI"), gateway_spy, dummy_notifier, std::move(std::get<0>(pm_tools)), nof_prbs);

  TESTASSERT(!gateway_spy.has_dl_tti_request_method_called());

  const mac_dl_sched_result& result = build_valid_mac_dl_sched_result();
  translator.on_new_downlink_scheduler_results(result);

  TESTASSERT(gateway_spy.has_dl_tti_request_method_called());
  const fapi::dl_tti_request_message& msg = gateway_spy.dl_tti_request_msg();
  TESTASSERT_EQ(msg.pdus.size(), 5U);
  TESTASSERT_EQ((msg.pdus.begin() + 1)->pdu_type, fapi::dl_pdu_type::PDCCH);
  TESTASSERT_EQ(msg.pdus.back().pdu_type, fapi::dl_pdu_type::PDSCH);
  TESTASSERT_EQ(msg.pdus.front().pdu_type, fapi::dl_pdu_type::PDCCH);
  TESTASSERT_EQ(msg.pdus.front().pdcch_pdu.dl_dci.size(), 3U);
  TESTASSERT_EQ((msg.pdus.begin() + 1)->pdcch_pdu.dl_dci.size(), 1U);
  TESTASSERT_EQ((msg.pdus.end() - 2)->pdu_type, fapi::dl_pdu_type::SSB);
  TESTASSERT_EQ((msg.pdus.end() - 3)->pdu_type, fapi::dl_pdu_type::SSB);

  const srsran::dl_ssb_pdu& pdu      = result.ssb_pdus.front();
  const fapi::dl_ssb_pdu&   fapi_pdu = (msg.pdus.end() - 3)->ssb_pdu;
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
  test_valid_dl_sched_results_generate_correct_dl_tti_request();
}
