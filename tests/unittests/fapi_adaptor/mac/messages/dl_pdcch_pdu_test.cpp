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

#include "helpers.h"
#include "srsran/fapi_adaptor/mac/messages/pdcch.h"
#include "srsran/fapi_adaptor/precoding_matrix_table_generator.h"
#include "srsran/mac/mac_cell_result.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace fapi_adaptor;
using namespace unittests;

TEST(mac_fapi_pdcch_pdu_conversor_test, mac_to_fapi_conversion_is_valid)
{
  unsigned                               nof_prbs    = 51;
  const mac_dl_sched_result_test_helper& result_test = build_valid_mac_dl_sched_result();
  const mac_dl_sched_result&             result      = result_test.result;
  const mac_pdcch_pdu&                   mac_pdu     = {result.dl_res->dl_pdcchs.front().ctx.bwp_cfg,
                                                        result.dl_res->dl_pdcchs.front().ctx.coreset_cfg,
                                                        result.dl_res->dl_pdcchs.front().ctx.starting_symbol,
                                                        {{&result.dl_res->dl_pdcchs.front().ctx, &result.dl_pdcch_pdus.front()}}};

  fapi::dl_pdcch_pdu fapi_pdu;
  auto               pm_tools = generate_precoding_matrix_tables(1);
  convert_pdcch_mac_to_fapi(fapi_pdu, mac_pdu, *std::get<0>(pm_tools), nof_prbs);

  // BWP.
  ASSERT_EQ(mac_pdu.bwp_cfg->cp, fapi_pdu.cp);
  ASSERT_EQ(mac_pdu.bwp_cfg->scs, fapi_pdu.scs);
  ASSERT_EQ(mac_pdu.coreset_cfg->coreset0_crbs().start(), fapi_pdu.coreset_bwp_start);
  ASSERT_EQ(mac_pdu.coreset_cfg->coreset0_crbs().length(), fapi_pdu.coreset_bwp_size);

  // CORESET.
  ASSERT_EQ(mac_pdu.coreset_cfg->duration, fapi_pdu.duration_symbols);
  ASSERT_EQ(mac_pdu.coreset_cfg->interleaved.has_value() ? fapi::cce_to_reg_mapping_type::interleaved
                                                         : fapi::cce_to_reg_mapping_type::non_interleaved,
            fapi_pdu.cce_reg_mapping_type);
  ASSERT_EQ(mac_pdu.coreset_cfg->interleaved.has_value() ? mac_pdu.coreset_cfg->interleaved.value().reg_bundle_sz : 6U,
            fapi_pdu.reg_bundle_size);
  ASSERT_EQ(mac_pdu.coreset_cfg->interleaved.has_value() ? mac_pdu.coreset_cfg->interleaved.value().interleaver_sz : 0U,
            fapi_pdu.interleaver_size);
  ASSERT_EQ(mac_pdu.coreset_cfg->interleaved.has_value() ? mac_pdu.coreset_cfg->interleaved.value().shift_index : 0U,
            fapi_pdu.shift_index);
  ASSERT_EQ(mac_pdu.coreset_cfg->precoder_granurality, fapi_pdu.precoder_granularity);
  ASSERT_EQ((mac_pdu.coreset_cfg->id == to_coreset_id(0)) ? fapi::pdcch_coreset_type::pbch_or_coreset0
                                                          : fapi::pdcch_coreset_type::other,
            fapi_pdu.coreset_type);

  if (mac_pdu.coreset_cfg->id != to_coreset_id(0)) {
    ASSERT_EQ(mac_pdu.coreset_cfg->freq_domain_resources(), fapi_pdu.freq_domain_resource);
  }

  // DCIs.
  const mac_pdcch_pdu::dci_info& dci = mac_pdu.dcis.front();
  ASSERT_EQ(dci.info->rnti, fapi_pdu.dl_dci.front().rnti);
  ASSERT_EQ(dci.info->n_id_pdcch_data, fapi_pdu.dl_dci.front().nid_pdcch_data);
  ASSERT_EQ(dci.info->n_rnti_pdcch_data, fapi_pdu.dl_dci.front().nrnti_pdcch_data);
  ASSERT_EQ(dci.info->cces.ncce, fapi_pdu.dl_dci.front().cce_index);
  ASSERT_EQ(dci.info->starting_symbol, fapi_pdu.start_symbol_index);
  ASSERT_EQ(to_nof_cces(dci.info->cces.aggr_lvl), fapi_pdu.dl_dci.front().aggregation_level);
  ASSERT_EQ(dci.info->n_id_pdcch_dmrs, fapi_pdu.parameters_v4.params.front().nid_pdcch_dmrs);
  ASSERT_EQ(*dci.payload, fapi_pdu.dl_dci.front().payload);
}
