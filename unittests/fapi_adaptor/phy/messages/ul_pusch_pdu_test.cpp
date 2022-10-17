/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "../../../fapi/validators/helpers.h"
#include "srsgnb/fapi_adaptor/phy/messages/pusch.h"
#include <gtest/gtest.h>

using namespace srsgnb;
using namespace fapi_adaptor;
using namespace unittest;

TEST(FapiPhyULPUSCHAdaptorTest, ValidPDUPass)
{
  fapi::ul_pusch_pdu fapi_pdu = build_valid_ul_pusch_pdu();

  unsigned sfn  = 1U;
  unsigned slot = 2U;

  uplink_processor::pusch_pdu pdu;
  convert_pusch_fapi_to_phy(pdu, fapi_pdu, sfn, slot);

  const pusch_processor::pdu_t& phy_pdu = pdu.pdu;
  ASSERT_EQ(slot_point(to_numerology_value(fapi_pdu.scs), sfn, slot), phy_pdu.slot);
  ASSERT_EQ(fapi_pdu.start_symbol_index, phy_pdu.start_symbol_index);
  ASSERT_EQ(fapi_pdu.nr_of_symbols, phy_pdu.nof_symbols);
  ASSERT_EQ(fapi_pdu.rnti, phy_pdu.rnti);
  ASSERT_EQ(fapi_pdu.bwp_start, phy_pdu.bwp_start_rb);
  ASSERT_EQ(fapi_pdu.bwp_size, phy_pdu.bwp_size_rb);
  ASSERT_EQ((fapi_pdu.cyclic_prefix == fapi::cyclic_prefix_type::normal) ? cyclic_prefix::NORMAL
                                                                         : cyclic_prefix::EXTENDED,
            phy_pdu.cp);
  ASSERT_EQ(fapi_pdu.qam_mod_order, phy_pdu.modulation);
  ASSERT_EQ(fapi_pdu.nid_pusch, phy_pdu.n_id);
  ASSERT_EQ(fapi_pdu.num_layers, phy_pdu.nof_tx_layers);

  // DMRS.
  for (unsigned i = 0; i != 14; ++i) {
    ASSERT_EQ(((fapi_pdu.ul_dmrs_symb_pos >> i) & 1U) == 1U, phy_pdu.dmrs_symbol_mask[i]);
  }
  ASSERT_EQ(dmrs_type((fapi_pdu.dmrs_type == fapi::dmrs_cfg_type::type_1) ? dmrs_type::options::TYPE1
                                                                          : dmrs_type::options::TYPE2),
            phy_pdu.dmrs);
  ASSERT_EQ(fapi_pdu.pusch_dmrs_scrambling_id, phy_pdu.scrambling_id);
  ASSERT_EQ(fapi_pdu.nscid, phy_pdu.n_scid);
  ASSERT_EQ(fapi_pdu.num_dmrs_cdm_grps_no_data, phy_pdu.nof_cdm_groups_without_data);
  ASSERT_EQ(fapi_pdu.pusch_maintenance_v3.tb_size_lbrm_bytes, phy_pdu.tbs_lbrm_bytes);

  // RB allocation.
  bounded_bitset<MAX_RB> vrb_bitmap(fapi_pdu.bwp_size);
  for (unsigned vrb_index = 0, vrb_index_end = fapi_pdu.bwp_size; vrb_index != vrb_index_end; ++vrb_index) {
    unsigned byte = vrb_index / 8;
    unsigned bit  = vrb_index % 8;
    if ((fapi_pdu.rb_bitmap[byte] >> bit) & 1U) {
      vrb_bitmap.set(vrb_index);
    }
  }

  rb_allocation alloc = (fapi_pdu.resource_alloc == fapi::resource_allocation_type::type_1)
                            ? rb_allocation::make_type1(fapi_pdu.rb_start, fapi_pdu.rb_size, {})
                            : rb_allocation::make_type0(vrb_bitmap, {});

  ASSERT_EQ(alloc, phy_pdu.freq_alloc);

  // Codeword.
  ASSERT_TRUE(phy_pdu.codeword.has_value());
  ASSERT_EQ(fapi_pdu.pusch_data.rv_index, phy_pdu.codeword.value().rv);
  ASSERT_EQ(fapi_pdu.pusch_data.new_data, phy_pdu.codeword.value().new_data);
  ASSERT_EQ(fapi_pdu.pusch_maintenance_v3.ldpc_base_graph, phy_pdu.codeword.value().ldpc_base_graph);
  ASSERT_EQ(fapi_pdu.pusch_data.tb_size, pdu.tb_size);
  ASSERT_EQ(fapi_pdu.pusch_data.harq_process_id, pdu.harq_id);
}
