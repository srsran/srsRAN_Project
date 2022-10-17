/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/fapi_adaptor/phy/messages/pusch.h"

using namespace srsgnb;
using namespace fapi_adaptor;

/// Fills the optional codeword description parameter of the PUSCH PDU, if present.
static void fill_codeword(uplink_processor::pusch_pdu& pdu, const fapi::ul_pusch_pdu& fapi_pdu)
{
  if (!fapi_pdu.pdu_bitmap[fapi::ul_pusch_pdu::PUSCH_DATA_BIT]) {
    return;
  }
  pusch_processor::codeword_description cw;
  cw.rv              = fapi_pdu.pusch_data.rv_index;
  cw.ldpc_base_graph = fapi_pdu.pusch_maintenance_v3.ldpc_base_graph;
  cw.new_data        = fapi_pdu.pusch_data.new_data;

  pdu.harq_id = fapi_pdu.pusch_data.harq_process_id;
  pdu.tb_size = fapi_pdu.pusch_data.tb_size;

  pdu.pdu.codeword = optional<pusch_processor::codeword_description>(std::move(cw));
}

/// Fills the \c rb_allocation parameter of the PUSCH PDU.
static void fill_rb_allocation(pusch_processor::pdu_t& proc_pdu, const fapi::ul_pusch_pdu& fapi_pdu)
{
  if (fapi_pdu.resource_alloc == fapi::resource_allocation_type::type_1) {
    proc_pdu.freq_alloc = rb_allocation::make_type1(fapi_pdu.rb_start, fapi_pdu.rb_size, {});
    return;
  }

  // Unpack the VRB bitmap. LSB of byte 0 of the bitmap represents the VRB 0.
  bounded_bitset<MAX_RB> vrb_bitmap(fapi_pdu.bwp_size);
  for (unsigned vrb_index = 0, vrb_index_end = fapi_pdu.bwp_size; vrb_index != vrb_index_end; ++vrb_index) {
    unsigned byte = vrb_index / 8;
    unsigned bit  = vrb_index % 8;
    if ((fapi_pdu.rb_bitmap[byte] >> bit) & 1U) {
      vrb_bitmap.set(vrb_index);
    }
  }

  proc_pdu.freq_alloc = rb_allocation::make_type0(vrb_bitmap, {});
}

void srsgnb::fapi_adaptor::convert_pusch_fapi_to_phy(uplink_processor::pusch_pdu& pdu,
                                                     const fapi::ul_pusch_pdu&    fapi_pdu,
                                                     uint16_t                     sfn,
                                                     uint16_t                     slot)
{
  // Fill the PUSCH processor parameters.
  pusch_processor::pdu_t& proc_pdu = pdu.pdu;
  proc_pdu.slot                    = slot_point(fapi_pdu.scs, sfn, slot);
  proc_pdu.rnti                    = fapi_pdu.rnti;
  proc_pdu.bwp_start_rb            = fapi_pdu.bwp_start;
  proc_pdu.bwp_size_rb             = fapi_pdu.bwp_size;

  proc_pdu.cp = cyclic_prefix((fapi_pdu.cyclic_prefix == fapi::cyclic_prefix_type::normal) ? cyclic_prefix::NORMAL
                                                                                           : cyclic_prefix::EXTENDED);
  proc_pdu.modulation    = fapi_pdu.qam_mod_order;
  proc_pdu.n_id          = fapi_pdu.nid_pusch;
  proc_pdu.nof_tx_layers = fapi_pdu.num_layers;

  // This value is defined in field DL DMRS symbol position in SCF-222 v4.0 Section 3.4.3.2, in table PUSCH PDU, DMRS
  // subsection.
  static constexpr unsigned DL_DMRS_SYMBOL_POS_SIZE = 14U;
  for (unsigned i = 0; i != DL_DMRS_SYMBOL_POS_SIZE; ++i) {
    proc_pdu.dmrs_symbol_mask[i] = ((static_cast<unsigned>(fapi_pdu.ul_dmrs_symb_pos >> i) & 1U) == 1U);
  }

  proc_pdu.dmrs =
      fapi_pdu.dmrs_type == fapi::dmrs_cfg_type::type_1 ? dmrs_type::options::TYPE1 : dmrs_type::options::TYPE2;
  proc_pdu.scrambling_id               = fapi_pdu.pusch_dmrs_scrambling_id;
  proc_pdu.n_scid                      = fapi_pdu.nscid;
  proc_pdu.nof_cdm_groups_without_data = fapi_pdu.num_dmrs_cdm_grps_no_data;
  proc_pdu.start_symbol_index          = fapi_pdu.start_symbol_index;
  proc_pdu.nof_symbols                 = fapi_pdu.nr_of_symbols;
  proc_pdu.tbs_lbrm_bytes              = fapi_pdu.pusch_maintenance_v3.tb_size_lbrm_bytes;

  fill_rb_allocation(proc_pdu, fapi_pdu);

  fill_codeword(pdu, fapi_pdu);

  // :TODO: Check the ports.
  proc_pdu.rx_ports = {0};
}
