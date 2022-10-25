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
using namespace fapi_adaptor;
using namespace unittests;

static void test_conversion_ok()
{
  const mac_dl_sched_result& result  = build_valid_mac_dl_sched_result();
  const mac_pdcch_pdu&       mac_pdu = {result.dl_res->dl_pdcchs.front().ctx.bwp_cfg,
                                        result.dl_res->dl_pdcchs.front().ctx.coreset_cfg,
                                        result.dl_res->dl_pdcchs.front().ctx.starting_symbol,
                                        {{&result.dl_res->dl_pdcchs.front().ctx, &result.dl_pdcch_pdus.front()}}};

  fapi::dl_pdcch_pdu fapi_pdu;
  convert_pdcch_mac_to_fapi(fapi_pdu, mac_pdu);

  // BWP.
  TESTASSERT_EQ(mac_pdu.bwp_cfg->cp_extended ? cyclic_prefix::EXTENDED : cyclic_prefix::NORMAL, fapi_pdu.cp);
  TESTASSERT_EQ(mac_pdu.bwp_cfg->scs, fapi_pdu.scs);
  TESTASSERT_EQ(mac_pdu.coreset_cfg->coreset0_crbs().start(), fapi_pdu.coreset_bwp_start);
  TESTASSERT_EQ(mac_pdu.coreset_cfg->coreset0_crbs().length(), fapi_pdu.coreset_bwp_size);

  // CORESET.
  TESTASSERT_EQ(0U, fapi_pdu.start_symbol_index);
  TESTASSERT_EQ(mac_pdu.coreset_cfg->duration, fapi_pdu.duration_symbols);
  TESTASSERT_EQ(mac_pdu.coreset_cfg->interleaved.has_value() ? fapi::cce_to_reg_mapping_type::interleaved
                                                             : fapi::cce_to_reg_mapping_type::non_interleaved,
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
  TESTASSERT_EQ((mac_pdu.coreset_cfg->id == to_coreset_id(0)) ? fapi::pdcch_coreset_type::pbch_or_coreset0
                                                              : fapi::pdcch_coreset_type::other,
                fapi_pdu.coreset_type);

  if (mac_pdu.coreset_cfg->id != to_coreset_id(0)) {
    TESTASSERT_EQ(mac_pdu.coreset_cfg->freq_domain_resources(), fapi_pdu.freq_domain_resource);
  }

  // DCIs.
  TESTASSERT_EQ(mac_pdu.dcis.front().parameters->rnti, fapi_pdu.dl_dci.front().rnti);
  TESTASSERT_EQ(mac_pdu.dcis.front().parameters->n_id_pdcch_data, fapi_pdu.dl_dci.front().nid_pdcch_data);
  TESTASSERT_EQ(mac_pdu.dcis.front().parameters->n_rnti_pdcch_data, fapi_pdu.dl_dci.front().nrnti_pdcch_data);
  TESTASSERT_EQ(mac_pdu.dcis.front().parameters->cces.ncce, fapi_pdu.dl_dci.front().cce_index);
  TESTASSERT_EQ(to_nof_cces(mac_pdu.dcis.front().parameters->cces.aggr_lvl), fapi_pdu.dl_dci.front().aggregation_level);
  TESTASSERT_EQ(mac_pdu.coreset_cfg->pdcch_dmrs_scrambling_id.has_value()
                    ? mac_pdu.coreset_cfg->pdcch_dmrs_scrambling_id.value()
                    : 0,
                fapi_pdu.parameters_v4.params.front().nid_pdcch_dmrs);
  TESTASSERT_EQ(*mac_pdu.dcis.front().payload, fapi_pdu.dl_dci.front().payload);
}

int main()
{
  test_conversion_ok();
}
