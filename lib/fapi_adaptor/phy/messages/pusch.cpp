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

#include "srsran/fapi_adaptor/phy/messages/pusch.h"
#include "srsran/ran/pusch/pusch_uci_beta_offset.h"

using namespace srsran;
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

/// Fills the \c UCI parameters of the PUSCH PDU.
static void fill_uci(pusch_processor::pdu_t& proc_pdu, const fapi::ul_pusch_pdu& fapi_pdu)
{
  if (!fapi_pdu.pdu_bitmap.test(fapi::ul_pusch_pdu::PUSCH_UCI_BIT)) {
    // Set every bitlength to 0.
    proc_pdu.uci.nof_harq_ack   = 0U;
    proc_pdu.uci.nof_csi_part1  = 0U;
    proc_pdu.uci.csi_part2_size = {};

    return;
  }

  // Fill the UCI parameters.
  pusch_processor::uci_description& phy_uci  = proc_pdu.uci;
  const fapi::ul_pusch_uci&         fapi_uci = fapi_pdu.pusch_uci;

  phy_uci.nof_harq_ack          = fapi_uci.harq_ack_bit_length;
  phy_uci.nof_csi_part1         = fapi_uci.csi_part1_bit_length;
  phy_uci.csi_part2_size        = {};
  phy_uci.alpha_scaling         = alpha_scaling_to_float(fapi_uci.alpha_scaling);
  phy_uci.beta_offset_harq_ack  = beta_harq_ack_to_float(fapi_uci.beta_offset_harq_ack);
  phy_uci.beta_offset_csi_part1 = beta_csi_to_float(fapi_uci.beta_offset_csi1);
  phy_uci.beta_offset_csi_part2 = beta_csi_to_float(fapi_uci.beta_offset_csi2);
}

void srsran::fapi_adaptor::convert_pusch_fapi_to_phy(uplink_processor::pusch_pdu& pdu,
                                                     const fapi::ul_pusch_pdu&    fapi_pdu,
                                                     uint16_t                     sfn,
                                                     uint16_t                     slot,
                                                     uint16_t                     num_rx_ant)
{
  // Fill the PUSCH processor parameters.
  pusch_processor::pdu_t& proc_pdu    = pdu.pdu;
  proc_pdu.slot                       = slot_point(fapi_pdu.scs, sfn, slot);
  proc_pdu.rnti                       = fapi_pdu.rnti;
  proc_pdu.bwp_start_rb               = fapi_pdu.bwp_start;
  proc_pdu.bwp_size_rb                = fapi_pdu.bwp_size;
  proc_pdu.cp                         = fapi_pdu.cp;
  proc_pdu.mcs_descr.modulation       = fapi_pdu.qam_mod_order;
  proc_pdu.mcs_descr.target_code_rate = fapi_pdu.target_code_rate * 0.1F;
  proc_pdu.n_id                       = fapi_pdu.nid_pusch;
  proc_pdu.nof_tx_layers              = fapi_pdu.num_layers;
  // This value is defined in field DL DMRS symbol position in SCF-222 v4.0 Section 3.4.3.2, in table PUSCH PDU, DMRS
  // subsection.
  static constexpr unsigned DL_DMRS_SYMBOL_POS_SIZE = 14U;
  for (unsigned i = 0; i != DL_DMRS_SYMBOL_POS_SIZE; ++i) {
    proc_pdu.dmrs_symbol_mask.set(i, ((static_cast<unsigned>(fapi_pdu.ul_dmrs_symb_pos >> i) & 1U) == 1U));
  }

  proc_pdu.dmrs =
      fapi_pdu.dmrs_type == fapi::dmrs_cfg_type::type_1 ? dmrs_type::options::TYPE1 : dmrs_type::options::TYPE2;
  proc_pdu.scrambling_id               = fapi_pdu.pusch_dmrs_scrambling_id;
  proc_pdu.n_scid                      = fapi_pdu.nscid;
  proc_pdu.nof_cdm_groups_without_data = fapi_pdu.num_dmrs_cdm_grps_no_data;
  proc_pdu.start_symbol_index          = fapi_pdu.start_symbol_index;
  proc_pdu.nof_symbols                 = fapi_pdu.nr_of_symbols;
  proc_pdu.tbs_lbrm_bytes              = fapi_pdu.pusch_maintenance_v3.tb_size_lbrm_bytes.value();

  if (fapi_pdu.tx_direct_current_location < 3300) {
    proc_pdu.dc_position = static_cast<unsigned>(fapi_pdu.tx_direct_current_location);
  }

  fill_rb_allocation(proc_pdu, fapi_pdu);

  fill_codeword(pdu, fapi_pdu);

  fill_uci(proc_pdu, fapi_pdu);

  proc_pdu.context = fapi_pdu.context;

  // Fill the antenna port indices starting from 0.
  proc_pdu.rx_ports.resize(num_rx_ant);
  std::iota(proc_pdu.rx_ports.begin(), proc_pdu.rx_ports.end(), 0);
}
