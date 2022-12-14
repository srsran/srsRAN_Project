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
#include "srsgnb/fapi_adaptor/mac/messages/pusch.h"
#include "srsgnb/phy/upper/channel_coding/ldpc/ldpc.h"
#include <gtest/gtest.h>

using namespace srsgnb;
using namespace fapi_adaptor;
using namespace unittests;

TEST(ULPUSCHPDUTest, ValidPUSCHShouldPass)
{
  const pusch_information& mac_pdu = build_valid_pusch_pdu();
  fapi::ul_pusch_pdu       fapi_pdu;

  convert_pusch_mac_to_fapi(fapi_pdu, mac_pdu);

  // BWP.
  ASSERT_EQ(mac_pdu.bwp_cfg->cp_extended ? cyclic_prefix::EXTENDED : cyclic_prefix::NORMAL, fapi_pdu.cp);
  ASSERT_EQ(mac_pdu.bwp_cfg->scs, fapi_pdu.scs);
  ASSERT_EQ(mac_pdu.bwp_cfg->crbs.start(), fapi_pdu.bwp_start);
  ASSERT_EQ(mac_pdu.bwp_cfg->crbs.length(), fapi_pdu.bwp_size);

  // Information parameters.
  ASSERT_EQ(static_cast<unsigned>(mac_pdu.mcs_descr.target_code_rate * 10.F), fapi_pdu.target_code_rate);
  ASSERT_EQ(mac_pdu.mcs_descr.modulation, fapi_pdu.qam_mod_order);
  ASSERT_EQ(mac_pdu.mcs_index.to_uint(), fapi_pdu.mcs_index);
  ASSERT_EQ(mac_pdu.mcs_table, fapi_pdu.mcs_table);
  ASSERT_EQ(mac_pdu.transform_precoding, fapi_pdu.transform_precoding);
  ASSERT_EQ(mac_pdu.n_id, fapi_pdu.nid_pusch);
  ASSERT_EQ(mac_pdu.nof_layers, fapi_pdu.num_layers);

  // DMRS.
  const dmrs_information& dmrs_cfg = mac_pdu.dmrs;
  ASSERT_EQ(dmrs_cfg.dmrs_symb_pos.to_uint64(), fapi_pdu.ul_dmrs_symb_pos);
  ASSERT_EQ((dmrs_cfg.config_type == srsgnb::dmrs_config_type::type1) ? fapi::dmrs_cfg_type::type_1
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
  ASSERT_EQ(mac_pdu.pusch_dmrs_id, fapi_pdu.pusch_dmrs_identity);

  // Frequency allocation.
  const prb_grant& prb_cfg = mac_pdu.prbs;
  ASSERT_TRUE(fapi_pdu.resource_alloc == fapi::resource_allocation_type::type_1);
  ASSERT_TRUE(fapi_pdu.vrb_to_prb_mapping == fapi::vrb_to_prb_mapping_type::non_interleaved);
  ASSERT_EQ(prb_cfg.prbs().start(), fapi_pdu.rb_start);
  ASSERT_EQ(prb_cfg.prbs().length(), fapi_pdu.rb_size);
  ASSERT_EQ(mac_pdu.intra_slot_freq_hopping, fapi_pdu.intra_slot_frequency_hopping);
  ASSERT_EQ(mac_pdu.tx_direct_current_location, fapi_pdu.tx_direct_current_location);
  ASSERT_EQ(mac_pdu.ul_freq_shift_7p5khz, fapi_pdu.uplink_frequency_shift_7p5kHz);

  // Time allocation.
  ASSERT_EQ(mac_pdu.symbols.start(), fapi_pdu.start_symbol_index);
  ASSERT_EQ(mac_pdu.symbols.length(), fapi_pdu.nr_of_symbols);

  // Maintenance v3.
  ASSERT_EQ(units::bytes{ldpc::MAX_CODEBLOCK_SIZE / 8}, fapi_pdu.pusch_maintenance_v3.tb_size_lbrm_bytes);
  ASSERT_EQ(mac_pdu.pusch_second_hop_prb, fapi_pdu.pusch_maintenance_v3.pusch_second_hop_prb);
  ASSERT_EQ(get_ldpc_base_graph(mac_pdu.mcs_descr.get_normalised_target_code_rate(),
                                units::bytes(mac_pdu.tb_size_bytes).to_bits()),
            fapi_pdu.pusch_maintenance_v3.ldpc_base_graph);
  ASSERT_EQ(static_cast<unsigned>(mac_pdu.dmrs_hopping_mode), fapi_pdu.pusch_maintenance_v3.group_or_sequence_hopping);

  // :TODO: add PUSCH Data optional.
  const fapi::ul_pusch_data& data = fapi_pdu.pusch_data;
  ASSERT_EQ(mac_pdu.rv_index, data.rv_index);
  ASSERT_EQ(mac_pdu.harq_id, data.harq_process_id);
  ASSERT_EQ(mac_pdu.new_data, data.new_data);
  ASSERT_EQ(mac_pdu.tb_size_bytes, data.tb_size.value());
  ASSERT_EQ(mac_pdu.num_cb, data.num_cb);
}
