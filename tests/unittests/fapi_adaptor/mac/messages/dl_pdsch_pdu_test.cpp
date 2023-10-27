/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "helpers.h"
#include "srsran/fapi_adaptor/mac/messages/pdsch.h"
#include "srsran/fapi_adaptor/precoding_matrix_table_generator.h"
#include "srsran/mac/mac_cell_result.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace fapi_adaptor;
using namespace unittests;

static void validate_pdsch_information(const pdsch_information& pdsch_cfg, const fapi::dl_pdsch_pdu& fapi_pdu)
{
  // BWP params
  const bwp_configuration& bwp_cfg = *pdsch_cfg.bwp_cfg;
  ASSERT_EQ(pdsch_cfg.coreset_cfg->coreset0_crbs().length(), fapi_pdu.bwp_size);
  ASSERT_EQ(pdsch_cfg.coreset_cfg->coreset0_crbs().start(), fapi_pdu.bwp_start);
  ASSERT_EQ(bwp_cfg.scs, fapi_pdu.scs);
  ASSERT_EQ(bwp_cfg.cp, fapi_pdu.cp);

  // Codewords.
  const auto& fapi_cw = fapi_pdu.cws.front();
  const auto& mac_cw  = pdsch_cfg.codewords.front();
  ASSERT_EQ(mac_cw.mcs_descr.target_code_rate * 10, fapi_cw.target_code_rate);
  ASSERT_EQ(mac_cw.rv_index, fapi_cw.rv_index);
  ASSERT_EQ(static_cast<unsigned>(mac_cw.mcs_table), fapi_cw.mcs_table);
  ASSERT_EQ(mac_cw.mcs_index, fapi_cw.mcs_index);
  ASSERT_EQ(get_bits_per_symbol(mac_cw.mcs_descr.modulation), fapi_cw.qam_mod_order);
  ASSERT_EQ(mac_cw.tb_size_bytes, fapi_cw.tb_size.value());

  // DMRS.
  const dmrs_information& dmrs_cfg = pdsch_cfg.dmrs;
  ASSERT_EQ(dmrs_cfg.dmrs_symb_pos.to_uint64(), fapi_pdu.dl_dmrs_symb_pos);
  ASSERT_EQ((dmrs_cfg.config_type == srsran::dmrs_config_type::type1) ? fapi::dmrs_cfg_type::type_1
                                                                      : fapi::dmrs_cfg_type::type_2,
            fapi_pdu.dmrs_type);
  ASSERT_EQ(dmrs_cfg.dmrs_scrambling_id, fapi_pdu.pdsch_dmrs_scrambling_id);
  ASSERT_EQ(dmrs_cfg.dmrs_scrambling_id_complement, fapi_pdu.pdsch_dmrs_scrambling_id_compl);
  ASSERT_EQ(dmrs_cfg.low_papr_dmrs ? fapi::low_papr_dmrs_type::dependent_cdm_group
                                   : fapi::low_papr_dmrs_type::independent_cdm_group,
            fapi_pdu.low_papr_dmrs);
  ASSERT_EQ(dmrs_cfg.n_scid, fapi_pdu.nscid);
  ASSERT_EQ(dmrs_cfg.num_dmrs_cdm_grps_no_data, fapi_pdu.num_dmrs_cdm_grps_no_data);
  ASSERT_EQ(dmrs_cfg.dmrs_ports.to_uint64(), fapi_pdu.dmrs_ports);

  // Frequency allocation.
  const vrb_alloc& prb_cfg = pdsch_cfg.rbs;
  ASSERT_EQ(fapi_pdu.resource_alloc, fapi::resource_allocation_type::type_1);
  ASSERT_EQ(fapi_pdu.vrb_to_prb_mapping, fapi::vrb_to_prb_mapping_type::non_interleaved);
  ASSERT_EQ(prb_cfg.type1().start(), fapi_pdu.rb_start);
  ASSERT_EQ(prb_cfg.type1().length(), fapi_pdu.rb_size);
}

TEST(mac_fapi_pdsch_pdu_conversor_test, sib1_mac_to_fapi_conversion_is_valid)
{
  sib_information pdu          = build_valid_sib1_information_pdu();
  unsigned        nof_csi_pdus = 2;
  unsigned        nof_prbs     = 51U;

  fapi::dl_pdsch_pdu fapi_pdu;
  auto               pm_tools = generate_precoding_matrix_tables(1);
  convert_pdsch_mac_to_fapi(fapi_pdu, pdu, nof_csi_pdus, *std::get<0>(pm_tools), nof_prbs);

  validate_pdsch_information(pdu.pdsch_cfg, fapi_pdu);

  // CSI-RS rm.
  span<const uint16_t> csi_rm = fapi_pdu.pdsch_maintenance_v3.csi_for_rm;
  ASSERT_EQ(nof_csi_pdus, csi_rm.size());
  for (unsigned i = 0; i != nof_csi_pdus; ++i) {
    ASSERT_EQ(i, csi_rm[i]);
  }
}

TEST(mac_fapi_pdsch_pdu_conversor_test, rar_mac_to_fapi_conversion_is_valid)
{
  rar_information pdu          = build_valid_rar_information_pdu();
  unsigned        nof_csi_pdus = 2;
  unsigned        nof_prbs     = 51U;

  fapi::dl_pdsch_pdu fapi_pdu;
  auto               pm_tools = generate_precoding_matrix_tables(1);
  convert_pdsch_mac_to_fapi(fapi_pdu, pdu, nof_csi_pdus, *std::get<0>(pm_tools), nof_prbs);

  validate_pdsch_information(pdu.pdsch_cfg, fapi_pdu);

  // CSI-RS rm.
  span<const uint16_t> csi_rm = fapi_pdu.pdsch_maintenance_v3.csi_for_rm;
  ASSERT_EQ(nof_csi_pdus, csi_rm.size());
  for (unsigned i = 0; i != nof_csi_pdus; ++i) {
    ASSERT_EQ(i, csi_rm[i]);
  }
}

TEST(mac_fapi_pdsch_pdu_conversor_test, dl_paging_mac_to_fapi_conversion_is_valid)
{
  dl_paging_allocation pdu          = build_valid_dl_paging_pdu();
  unsigned             nof_csi_pdus = 2;
  unsigned             nof_prbs     = 51U;

  fapi::dl_pdsch_pdu fapi_pdu;
  auto               pm_tools = generate_precoding_matrix_tables(1);
  convert_pdsch_mac_to_fapi(fapi_pdu, pdu, nof_csi_pdus, *std::get<0>(pm_tools), nof_prbs);

  validate_pdsch_information(pdu.pdsch_cfg, fapi_pdu);

  // CSI-RS rm.
  span<const uint16_t> csi_rm = fapi_pdu.pdsch_maintenance_v3.csi_for_rm;
  ASSERT_EQ(nof_csi_pdus, csi_rm.size());
  for (unsigned i = 0; i != nof_csi_pdus; ++i) {
    ASSERT_EQ(i, csi_rm[i]);
  }
}

TEST(mac_fapi_pdsch_pdu_conversor_test, dl_msg_alloc_mac_to_fapi_conversion_is_valid)
{
  dl_msg_alloc pdu          = build_valid_dl_msg_alloc_pdu();
  unsigned     nof_csi_pdus = 2;
  unsigned     nof_prbs     = 51U;

  fapi::dl_pdsch_pdu fapi_pdu;
  auto               pm_tools = generate_precoding_matrix_tables(1);
  convert_pdsch_mac_to_fapi(fapi_pdu, pdu, nof_csi_pdus, *std::get<0>(pm_tools), nof_prbs);

  validate_pdsch_information(pdu.pdsch_cfg, fapi_pdu);

  // CSI-RS rm.
  span<const uint16_t> csi_rm = fapi_pdu.pdsch_maintenance_v3.csi_for_rm;
  ASSERT_EQ(nof_csi_pdus, csi_rm.size());
  for (unsigned i = 0; i != nof_csi_pdus; ++i) {
    ASSERT_EQ(i, csi_rm[i]);
  }
}
