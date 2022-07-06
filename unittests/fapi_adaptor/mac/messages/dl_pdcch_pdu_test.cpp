/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "helpers.h"
#include "srsgnb/fapi_adaptor/mac/messages/pdcch.h"
#include "srsgnb/mac/mac_cell_result.h"
#include "srsgnb/support/srsgnb_test.h"

using namespace srsgnb;
using namespace fapi;
using namespace fapi_adaptor;
using namespace unittests;

static void test_conversion_ok()
{
  const mac_dl_sched_result& result  = build_valid_mac_dl_sched_result();
  const mac_pdcch_pdu&       mac_pdu = {result.dl_res->dl_pdcchs.front().ctx.bwp_cfg,
                                        result.dl_res->dl_pdcchs.front().ctx.coreset_cfg,
                                        {{&result.dl_res->dl_pdcchs.front(), &result.pdcch_pdus.front()}}};

  dl_pdcch_pdu fapi_pdu;
  convert_pdcch_mac_to_fapi(fapi_pdu, mac_pdu);

  // BWP.
  TESTASSERT_EQ(mac_pdu.bwp_cfg->cp_extended ? cyclic_prefix_type::extended : cyclic_prefix_type::normal,
                fapi_pdu.cyclic_prefix);
  TESTASSERT_EQ(mac_pdu.bwp_cfg->scs, fapi_pdu.scs);
  TESTASSERT_EQ(mac_pdu.bwp_cfg->crbs.start(), fapi_pdu.coreset_bwp_start);
  TESTASSERT_EQ(mac_pdu.bwp_cfg->crbs.length(), fapi_pdu.coreset_bwp_size);

  // CORESET.
  TESTASSERT_EQ(0U, fapi_pdu.start_symbol_index);
  TESTASSERT_EQ(mac_pdu.coreset_cfg->duration, fapi_pdu.duration_symbols);
  TESTASSERT_EQ(mac_pdu.coreset_cfg->interleaved.has_value() ? cce_to_reg_mapping_type::interleaved
                                                             : cce_to_reg_mapping_type::non_interleaved,
                fapi_pdu.cce_reg_mapping_type);
  TESTASSERT_EQ(mac_pdu.coreset_cfg->interleaved.has_value() ? mac_pdu.coreset_cfg->interleaved.value().reg_bundle_sz
                                                             : 6U,
                fapi_pdu.reg_bundle_size);
  TESTASSERT_EQ(mac_pdu.coreset_cfg->interleaved.has_value() ? mac_pdu.coreset_cfg->interleaved.value().interleaver_sz
                                                             : 0U,
                fapi_pdu.interleaver_size);
  TESTASSERT_EQ(mac_pdu.coreset_cfg->interleaved.has_value() ? mac_pdu.coreset_cfg->interleaved.value().shift_index
                                                             : 0U,
                fapi_pdu.shift_index);
  TESTASSERT_EQ(mac_pdu.coreset_cfg->precoder_granurality, fapi_pdu.precoder_granularity);
  TESTASSERT_EQ((mac_pdu.dcis.front().parameters->dci.type == dci_dl_rnti_config_type::si_f1_0)
                    ? pdcch_coreset_type::pbch_or_sib1
                    : pdcch_coreset_type::other,
                fapi_pdu.coreset_type);

  const freq_resource_bitmap& mac_freq = mac_pdu.coreset_cfg->freq_domain_resources();
  for (unsigned i = 0, e = mac_freq.size(); i != e; ++i) {
    TESTASSERT_EQ(mac_freq.test(i), bool((fapi_pdu.freq_domain_resource[i / 8] >> i % 8) & 1U));
  }

  // DCIs.
  TESTASSERT_EQ(mac_pdu.dcis.front().parameters->ctx.rnti, fapi_pdu.dl_dci.front().rnti);
  TESTASSERT_EQ(mac_pdu.dcis.front().parameters->ctx.n_id_pdcch_data, fapi_pdu.dl_dci.front().nid_pdcch_data);
  TESTASSERT_EQ(mac_pdu.dcis.front().parameters->ctx.n_rnti_pdcch_data, fapi_pdu.dl_dci.front().nrnti_pdcch_data);
  TESTASSERT_EQ(mac_pdu.dcis.front().parameters->ctx.cces.ncce, fapi_pdu.dl_dci.front().cce_index);
  TESTASSERT_EQ(to_nof_cces(mac_pdu.dcis.front().parameters->ctx.cces.aggr_lvl),
                fapi_pdu.dl_dci.front().aggregation_level);
  TESTASSERT_EQ(mac_pdu.coreset_cfg->pdcch_dmrs_scrambling_id, fapi_pdu.parameters_v4.params.front().nid_pdcch_dmrs);

  const dci_payload&               mac_pay  = *mac_pdu.dcis.front().payload;
  const static_vector<uint8_t, 16> fapi_pay = fapi_pdu.dl_dci.front().payload;
  for (unsigned i = 0, e = mac_pay.size(); i != e; ++i) {
    TESTASSERT_EQ(mac_pay.test(i), bool((fapi_pay[i / 8] >> i % 8) & 1U));
  }
}

int main()
{
  test_conversion_ok();
}
