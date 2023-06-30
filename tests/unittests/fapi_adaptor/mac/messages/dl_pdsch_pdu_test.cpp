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
#include "srsran/fapi_adaptor/mac/messages/pdsch.h"
#include "srsran/fapi_adaptor/precoding_matrix_table_generator.h"
#include "srsran/mac/mac_cell_result.h"
#include "srsran/support/srsran_test.h"

using namespace srsran;
using namespace fapi_adaptor;
using namespace unittests;

static void test_conversion_ok()
{
  sib_information pdu          = build_valid_sib1_information_pdu();
  unsigned        nof_csi_pdus = 2;
  unsigned        nof_prbs     = 51U;

  fapi::dl_pdsch_pdu fapi_pdu;
  auto               pm_tools = generate_precoding_matrix_tables(1);
  convert_pdsch_mac_to_fapi(fapi_pdu, pdu, nof_csi_pdus, *std::get<0>(pm_tools), nof_prbs);

  // BWP params
  const bwp_configuration& bwp_cfg = *pdu.pdsch_cfg.bwp_cfg;
  TESTASSERT_EQ(pdu.pdsch_cfg.coreset_cfg->coreset0_crbs().length(), fapi_pdu.bwp_size);
  TESTASSERT_EQ(pdu.pdsch_cfg.coreset_cfg->coreset0_crbs().start(), fapi_pdu.bwp_start);
  TESTASSERT_EQ(bwp_cfg.scs, fapi_pdu.scs);
  TESTASSERT_EQ(bwp_cfg.cp, fapi_pdu.cp);

  // Codewords.
  const auto& fapi_cw = fapi_pdu.cws.front();
  const auto& mac_cw  = pdu.pdsch_cfg.codewords.front();
  TESTASSERT_EQ(mac_cw.mcs_descr.target_code_rate * 10, fapi_cw.target_code_rate);
  TESTASSERT_EQ(mac_cw.rv_index, fapi_cw.rv_index);
  TESTASSERT_EQ(static_cast<unsigned>(mac_cw.mcs_table), fapi_cw.mcs_table);
  TESTASSERT_EQ(mac_cw.mcs_index, fapi_cw.mcs_index);
  TESTASSERT_EQ(get_bits_per_symbol(mac_cw.mcs_descr.modulation), fapi_cw.qam_mod_order);
  TESTASSERT_EQ(mac_cw.tb_size_bytes, fapi_cw.tb_size.value());

  // DMRS.
  const dmrs_information& dmrs_cfg = pdu.pdsch_cfg.dmrs;
  TESTASSERT_EQ(dmrs_cfg.dmrs_symb_pos.to_uint64(), fapi_pdu.dl_dmrs_symb_pos);
  TESTASSERT_EQ((dmrs_cfg.config_type == srsran::dmrs_config_type::type1) ? fapi::dmrs_cfg_type::type_1
                                                                          : fapi::dmrs_cfg_type::type_2,
                fapi_pdu.dmrs_type);
  TESTASSERT_EQ(dmrs_cfg.dmrs_scrambling_id, fapi_pdu.pdsch_dmrs_scrambling_id);
  TESTASSERT_EQ(dmrs_cfg.dmrs_scrambling_id_complement, fapi_pdu.pdsch_dmrs_scrambling_id_compl);
  TESTASSERT_EQ(dmrs_cfg.low_papr_dmrs ? fapi::low_papr_dmrs_type::dependent_cdm_group
                                       : fapi::low_papr_dmrs_type::independent_cdm_group,
                fapi_pdu.low_papr_dmrs);
  TESTASSERT_EQ(dmrs_cfg.n_scid, fapi_pdu.nscid);
  TESTASSERT_EQ(dmrs_cfg.num_dmrs_cdm_grps_no_data, fapi_pdu.num_dmrs_cdm_grps_no_data);
  TESTASSERT_EQ(dmrs_cfg.dmrs_ports.to_uint64(), fapi_pdu.dmrs_ports);

  // Frequency allocation.
  const vrb_alloc& prb_cfg = pdu.pdsch_cfg.rbs;
  TESTASSERT(fapi_pdu.resource_alloc == fapi::resource_allocation_type::type_1);
  TESTASSERT(fapi_pdu.vrb_to_prb_mapping == fapi::vrb_to_prb_mapping_type::non_interleaved);
  TESTASSERT_EQ(prb_cfg.type1().start(), fapi_pdu.rb_start);
  TESTASSERT_EQ(prb_cfg.type1().length(), fapi_pdu.rb_size);

  // CSI-RS rm.
  const static_vector<uint16_t, 16>& csi_rm = fapi_pdu.pdsch_maintenance_v3.csi_for_rm;
  TESTASSERT_EQ(nof_csi_pdus, csi_rm.size());
  for (unsigned i = 0; i != nof_csi_pdus; ++i) {
    TESTASSERT_EQ(i, csi_rm[i]);
  }
}

int main()
{
  test_conversion_ok();
}
