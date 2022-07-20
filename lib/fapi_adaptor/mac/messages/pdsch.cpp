/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/fapi_adaptor/mac/messages/pdsch.h"
#include "srsgnb/mac/mac_cell_result.h"
#include "srsgnb/phy/upper/channel_coding/ldpc/ldpc.h"

using namespace srsgnb;
using namespace fapi;
using namespace fapi_adaptor;

void srsgnb::fapi_adaptor::convert_pdsch_mac_to_fapi(fapi::dl_pdsch_pdu& fapi_pdu, const sib_information& mac_pdu)
{
  dl_pdsch_pdu_builder builder(fapi_pdu);

  convert_pdsch_mac_to_fapi(builder, mac_pdu);
}

static fapi::dmrs_config_type convert_dmrs_type_mac_to_fapi(srsgnb::dmrs_config_type type)
{
  switch (type) {
    case srsgnb::dmrs_config_type::type1:
      return fapi::dmrs_config_type::type_1;
    case srsgnb::dmrs_config_type::type2:
      return fapi::dmrs_config_type::type_2;
    default:
      srsran_assert(0, "Unexpected DMRS type ({})", static_cast<unsigned>(type));
      break;
  }
  // Fallback value.
  return fapi::dmrs_config_type::type_1;
}

void srsgnb::fapi_adaptor::convert_pdsch_mac_to_fapi(fapi::dl_pdsch_pdu_builder& builder,
                                                     const sib_information&      mac_pdu)
{
  srsran_assert(mac_pdu.pdsch_cfg.codewords.size() == 1, "This version only supports one transport block");

  // Basic parameters.
  builder.set_basic_parameters(mac_pdu.pdsch_cfg.rnti);

  // BWP parameters.
  // NOTE: as it is a SIB1 PDU, use the CORESET BWP parameters.
  builder.set_bwp_parameters(mac_pdu.pdcch_cfg->ctx.coreset_cfg->coreset0_crbs().length(),
                             mac_pdu.pdcch_cfg->ctx.coreset_cfg->coreset0_crbs().start(),
                             mac_pdu.pdsch_cfg.bwp_cfg->scs,
                             mac_pdu.pdsch_cfg.bwp_cfg->cp_extended ? cyclic_prefix_type::extended
                                                                    : cyclic_prefix_type::normal);

  // Codewords.
  for (const auto& cw : mac_pdu.pdsch_cfg.codewords) {
    dl_pdsch_codeword_builder cw_builder = builder.add_codeword();
    cw_builder.set_basic_parameters(cw.target_code_rate,
                                    to_qam_modulation_order(cw.qam_mod),
                                    cw.mcs_index,
                                    static_cast<unsigned>(cw.mcs_table),
                                    cw.rv_index,
                                    cw.tb_size_bytes);
  }

  static const unsigned transmision_scheme = 0;
  static const unsigned num_layers         = 1;
  // Set nid_pdsch to pci.
  static const unsigned nid_pdsch = 1;
  builder.set_codeword_information_parameters(
      nid_pdsch, num_layers, transmision_scheme, pdsch_ref_point_type::subcarrier_0);

  // DMRS.
  const dmrs_information& dmrs_cfg = mac_pdu.pdsch_cfg.dmrs;
  builder.set_dmrs_parameters(dmrs_cfg.dmrs_symb_pos.to_uint64(),
                              convert_dmrs_type_mac_to_fapi(dmrs_cfg.config_type),
                              dmrs_cfg.dmrs_scrambling_id,
                              dmrs_cfg.dmrs_scrambling_id_complement,
                              dmrs_cfg.low_papr_dmrs ? low_papr_dmrs_type::dependent_cdm_group
                                                     : low_papr_dmrs_type::independent_cdm_group,
                              dmrs_cfg.n_scid,
                              dmrs_cfg.num_dmrs_cdm_grps_no_data,
                              dmrs_cfg.dmrs_ports.to_uint64());

  if (mac_pdu.pdsch_cfg.prbs.is_alloc_type0()) {
    static_vector<uint8_t, dl_pdsch_pdu::MAX_SIZE_RB_BITMAP> rb_map;
    rb_map.resize(dl_pdsch_pdu::MAX_SIZE_RB_BITMAP, 0U);
    const rbg_bitmap& mac_rbg_map = mac_pdu.pdsch_cfg.prbs.rbgs();
    for (unsigned i = 0, e = mac_rbg_map.size(); i != e; ++i) {
      rb_map[i / 8] |= uint8_t(mac_rbg_map.test(i) ? 1U : 0U) << i % 8;
    }
    builder.set_pdsch_allocation_in_frequency_type_0({rb_map}, vrb_to_prb_mapping_type::non_interleaved);

  } else {
    const prb_interval& prb_int = mac_pdu.pdsch_cfg.prbs.prbs();
    builder.set_pdsch_allocation_in_frequency_type_1(
        prb_int.start(), prb_int.length(), vrb_to_prb_mapping_type::non_interleaved);
  }

  builder.set_pdsch_allocation_in_time_parameters(mac_pdu.pdsch_cfg.symbols.start(),
                                                  mac_pdu.pdsch_cfg.symbols.length());

  // Power values.
  builder.set_tx_power_info_parameters({0}, nzp_csi_rs_epre_to_ssb::dB0);

  const coreset_configuration& coreset_cfg = *mac_pdu.pdcch_cfg->ctx.coreset_cfg;
  builder.set_maintenance_v3_bwp_parameters(pdsch_trans_type::non_interleaved_common_ss,
                                            coreset_cfg.coreset0_crbs().start(),
                                            coreset_cfg.coreset0_crbs().length());

  unsigned              tb_size_lbrm_bytes = ldpc::MAX_CODEBLOCK_SIZE / 8;
  const pdsch_codeword& cw                 = mac_pdu.pdsch_cfg.codewords.front();

  // NOTE: MAC uses the value of the target code rate x[1024], as per TS 38.214, Section 5.1.3.1, table 5.1.3.1-1.
  float R = cw.target_code_rate * (1.F / 1024);
  builder.set_maintenance_v3_codeword_parameters(
      get_ldpc_base_graph(R, cw.tb_size_bytes), tb_size_lbrm_bytes, false, false);

  // :TODO Rate-Matching related parameters, not used now.

  // Default v3 powers.
  builder.set_maintenance_v3_tx_power_info_parameters({}, {});
}
