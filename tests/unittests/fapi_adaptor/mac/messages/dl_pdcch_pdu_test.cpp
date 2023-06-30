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
#include "srsran/fapi_adaptor/mac/messages/pdcch.h"
#include "srsran/fapi_adaptor/precoding_matrix_table_generator.h"
#include "srsran/mac/mac_cell_result.h"
#include "srsran/support/srsran_test.h"

using namespace srsran;
using namespace fapi_adaptor;
using namespace unittests;

static void test_conversion_ok()
{
  unsigned                   nof_prbs = 51;
  const mac_dl_sched_result& result   = build_valid_mac_dl_sched_result();
  const mac_pdcch_pdu&       mac_pdu  = {result.dl_res->dl_pdcchs.front().ctx.bwp_cfg,
                                         result.dl_res->dl_pdcchs.front().ctx.coreset_cfg,
                                         result.dl_res->dl_pdcchs.front().ctx.starting_symbol,
                                         {{&result.dl_res->dl_pdcchs.front().ctx, &result.dl_pdcch_pdus.front()}}};

  fapi::dl_pdcch_pdu fapi_pdu;
  auto               pm_tools = generate_precoding_matrix_tables(1);
  convert_pdcch_mac_to_fapi(fapi_pdu, mac_pdu, *std::get<0>(pm_tools), nof_prbs);

  // BWP.
  TESTASSERT_EQ(mac_pdu.bwp_cfg->cp, fapi_pdu.cp);
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
  const mac_pdcch_pdu::dci_info& dci = mac_pdu.dcis.front();
  TESTASSERT_EQ(dci.info->rnti, fapi_pdu.dl_dci.front().rnti);
  TESTASSERT_EQ(dci.info->n_id_pdcch_data, fapi_pdu.dl_dci.front().nid_pdcch_data);
  TESTASSERT_EQ(dci.info->n_rnti_pdcch_data, fapi_pdu.dl_dci.front().nrnti_pdcch_data);
  TESTASSERT_EQ(dci.info->cces.ncce, fapi_pdu.dl_dci.front().cce_index);
  TESTASSERT_EQ(to_nof_cces(dci.info->cces.aggr_lvl), fapi_pdu.dl_dci.front().aggregation_level);
  TESTASSERT_EQ(dci.info->n_id_pdcch_dmrs, fapi_pdu.parameters_v4.params.front().nid_pdcch_dmrs);
  TESTASSERT_EQ(*dci.payload, fapi_pdu.dl_dci.front().payload);
}

int main()
{
  test_conversion_ok();
}
