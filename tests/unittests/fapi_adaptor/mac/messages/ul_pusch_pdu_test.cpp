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
#include "srsran/fapi_adaptor/mac/messages/pusch.h"
#include "srsran/fapi_adaptor/uci_part2_correspondence_generator.h"
#include "srsran/phy/upper/channel_coding/ldpc/ldpc.h"
#include <gtest/gtest.h>

using namespace srsran;
using namespace fapi_adaptor;
using namespace unittests;

TEST(mac_to_fapi_pusch_pdu_test, valid_pusch_pdu_shoul_pass)
{
  const ul_sched_info_test_helper& pdu_test = build_valid_pusch_pdu();
  const ul_sched_info&             mac_pdu  = pdu_test.info;
  fapi::ul_pusch_pdu               fapi_pdu;
  auto                             uci_part2_tools = generate_uci_part2_correspondence(1);

  convert_pusch_mac_to_fapi(fapi_pdu, mac_pdu, *std::get<0>(uci_part2_tools));

  // BWP.
  const pusch_information& pusch_cfg = mac_pdu.pusch_cfg;
  ASSERT_EQ(pusch_cfg.bwp_cfg->cp, fapi_pdu.cp);
  ASSERT_EQ(pusch_cfg.bwp_cfg->scs, fapi_pdu.scs);
  ASSERT_EQ(pusch_cfg.bwp_cfg->crbs.start(), fapi_pdu.bwp_start);
  ASSERT_EQ(pusch_cfg.bwp_cfg->crbs.length(), fapi_pdu.bwp_size);

  // Information parameters.
  ASSERT_EQ(static_cast<unsigned>(pusch_cfg.mcs_descr.target_code_rate * 10.F), fapi_pdu.target_code_rate);
  ASSERT_EQ(pusch_cfg.mcs_descr.modulation, fapi_pdu.qam_mod_order);
  ASSERT_EQ(pusch_cfg.mcs_index.to_uint(), fapi_pdu.mcs_index);
  ASSERT_EQ(pusch_cfg.mcs_table, fapi_pdu.mcs_table);
  ASSERT_EQ(pusch_cfg.transform_precoding, fapi_pdu.transform_precoding);
  ASSERT_EQ(pusch_cfg.n_id, fapi_pdu.nid_pusch);
  ASSERT_EQ(pusch_cfg.nof_layers, fapi_pdu.num_layers);

  // DMRS.
  const dmrs_information& dmrs_cfg = pusch_cfg.dmrs;
  ASSERT_EQ(dmrs_cfg.dmrs_symb_pos.to_uint64(), fapi_pdu.ul_dmrs_symb_pos);
  ASSERT_EQ((dmrs_cfg.config_type == srsran::dmrs_config_type::type1) ? fapi::dmrs_cfg_type::type_1
                                                                      : fapi::dmrs_cfg_type::type_2,
            fapi_pdu.dmrs_type);
  ASSERT_EQ(dmrs_cfg.dmrs_scrambling_id, fapi_pdu.pusch_dmrs_scrambling_id);
  ASSERT_EQ(dmrs_cfg.dmrs_scrambling_id_complement, fapi_pdu.pusch_dmrs_scrambling_id_complement);
  ASSERT_EQ(dmrs_cfg.low_papr_dmrs ? fapi::low_papr_dmrs_type::dependent_cdm_group
                                   : fapi::low_papr_dmrs_type::independent_cdm_group,
            fapi_pdu.low_papr_dmrs);
  ASSERT_EQ(dmrs_cfg.n_scid, fapi_pdu.nscid);
  ASSERT_EQ(dmrs_cfg.num_dmrs_cdm_grps_no_data, fapi_pdu.num_dmrs_cdm_grps_no_data);
  ASSERT_EQ(dmrs_cfg.dmrs_ports.to_uint64(), fapi_pdu.dmrs_ports);
  ASSERT_EQ(pusch_cfg.pusch_dmrs_id, fapi_pdu.pusch_dmrs_identity);

  // Frequency allocation.
  const vrb_alloc& prb_cfg = pusch_cfg.rbs;
  ASSERT_TRUE(fapi_pdu.resource_alloc == fapi::resource_allocation_type::type_1);
  ASSERT_TRUE(fapi_pdu.vrb_to_prb_mapping == fapi::vrb_to_prb_mapping_type::non_interleaved);
  ASSERT_EQ(prb_cfg.type1().start(), fapi_pdu.rb_start);
  ASSERT_EQ(prb_cfg.type1().length(), fapi_pdu.rb_size);
  ASSERT_EQ(pusch_cfg.intra_slot_freq_hopping, fapi_pdu.intra_slot_frequency_hopping);
  ASSERT_EQ(pusch_cfg.tx_direct_current_location, fapi_pdu.tx_direct_current_location);
  ASSERT_EQ(pusch_cfg.ul_freq_shift_7p5khz, fapi_pdu.uplink_frequency_shift_7p5kHz);

  // Time allocation.
  ASSERT_EQ(pusch_cfg.symbols.start(), fapi_pdu.start_symbol_index);
  ASSERT_EQ(pusch_cfg.symbols.length(), fapi_pdu.nr_of_symbols);

  // Maintenance v3.
  ASSERT_EQ(units::bits(ldpc::MAX_CODEBLOCK_SIZE).truncate_to_bytes(),
            fapi_pdu.pusch_maintenance_v3.tb_size_lbrm_bytes);
  ASSERT_EQ(pusch_cfg.pusch_second_hop_prb, fapi_pdu.pusch_maintenance_v3.pusch_second_hop_prb);
  ASSERT_EQ(get_ldpc_base_graph(pusch_cfg.mcs_descr.get_normalised_target_code_rate(),
                                units::bytes(pusch_cfg.tb_size_bytes).to_bits()),
            fapi_pdu.pusch_maintenance_v3.ldpc_base_graph);
  ASSERT_EQ(static_cast<unsigned>(pusch_cfg.dmrs_hopping_mode),
            fapi_pdu.pusch_maintenance_v3.group_or_sequence_hopping);

  const fapi::ul_pusch_data& data = fapi_pdu.pusch_data;
  ASSERT_EQ(pusch_cfg.rv_index, data.rv_index);
  ASSERT_EQ(pusch_cfg.harq_id, data.harq_process_id);
  ASSERT_EQ(pusch_cfg.new_data, data.new_data);
  ASSERT_EQ(pusch_cfg.tb_size_bytes, data.tb_size.value());
  ASSERT_EQ(pusch_cfg.num_cb, data.num_cb);

  const fapi::ul_pusch_uci& fapi_uci = fapi_pdu.pusch_uci;
  const uci_info&           mac_uci  = mac_pdu.uci.value();
  ASSERT_EQ(mac_uci.harq.has_value() ? mac_uci.harq->harq_ack_nof_bits : 0U, fapi_uci.harq_ack_bit_length);
  ASSERT_EQ(mac_uci.csi.has_value() ? mac_uci.csi->csi_part1_nof_bits : 0U, fapi_uci.csi_part1_bit_length);
  ASSERT_EQ(mac_uci.csi.has_value() && mac_uci.csi->beta_offset_csi_2.has_value() ? 65535U : 0U,
            fapi_uci.flags_csi_part2);
  ASSERT_EQ(mac_uci.alpha, fapi_uci.alpha_scaling);
  ASSERT_EQ(mac_uci.harq.has_value() ? mac_uci.harq->beta_offset_harq_ack : 0U, fapi_uci.beta_offset_harq_ack);
  ASSERT_EQ(mac_uci.csi.has_value() ? mac_uci.csi->beta_offset_csi_1 : 0U, fapi_uci.beta_offset_csi1);
  ASSERT_EQ(mac_uci.csi.has_value() && mac_uci.csi->beta_offset_csi_2.has_value()
                ? mac_uci.csi->beta_offset_csi_2.value()
                : 0U,
            fapi_uci.beta_offset_csi2);
}
