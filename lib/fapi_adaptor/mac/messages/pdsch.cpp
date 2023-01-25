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
using namespace fapi_adaptor;

/// CORESET0 is configured for the cell.
static constexpr bool is_coreset0_configured_for_cell = true;

void srsgnb::fapi_adaptor::convert_pdsch_mac_to_fapi(fapi::dl_pdsch_pdu& fapi_pdu, const sib_information& mac_pdu)
{
  fapi::dl_pdsch_pdu_builder builder(fapi_pdu);

  convert_pdsch_mac_to_fapi(builder, mac_pdu);
}

static crb_interval get_crb_interval(const pdsch_information& pdsch_cfg)
{
  if (pdsch_cfg.coreset_cfg->id == to_coreset_id(0)) {
    return pdsch_cfg.coreset_cfg->coreset0_crbs();
  }
  return pdsch_cfg.bwp_cfg->crbs;
}

static void fill_codewords(fapi::dl_pdsch_pdu_builder& builder, span<const pdsch_codeword> codewords)
{
  srsgnb_assert(codewords.size() == 1, "Current FAPI implementation only supports 1 transport block per PDU");
  for (const auto& cw : codewords) {
    fapi::dl_pdsch_codeword_builder cw_builder = builder.add_codeword();
    cw_builder.set_basic_parameters(cw.mcs_descr.target_code_rate,
                                    get_bits_per_symbol(cw.mcs_descr.modulation),
                                    cw.mcs_index.to_uint(),
                                    static_cast<unsigned>(cw.mcs_table),
                                    cw.rv_index,
                                    units::bytes{cw.tb_size_bytes});
  }

  const units::bytes    tb_size_lbrm_bytes           = units::bits(ldpc::MAX_CODEBLOCK_SIZE).truncate_to_bytes();
  const pdsch_codeword& cw                           = codewords.front();
  static const bool     is_tb_crc_first_tb_required  = false;
  static const bool     is_tb_crc_second_tb_required = false;

  // NOTE: MAC uses the value of the target code rate x[1024], as per TS 38.214, Section 5.1.3.1, table 5.1.3.1-1.
  float R = cw.mcs_descr.get_normalised_target_code_rate();
  builder.set_maintenance_v3_codeword_parameters(get_ldpc_base_graph(R, units::bytes{cw.tb_size_bytes}.to_bits()),
                                                 tb_size_lbrm_bytes,
                                                 is_tb_crc_first_tb_required,
                                                 is_tb_crc_second_tb_required);
}

static void
fill_codeword_information(fapi::dl_pdsch_pdu_builder& builder, unsigned nid_pdsch, fapi::pdsch_ref_point_type ref_point)
{
  static const unsigned transmision_scheme = 0;
  static const unsigned num_layers         = 1;
  builder.set_codeword_information_parameters(nid_pdsch, num_layers, transmision_scheme, ref_point);
}

static void fill_dmrs(fapi::dl_pdsch_pdu_builder& builder, const dmrs_information& dmrs)
{
  builder.set_dmrs_parameters(dmrs.dmrs_symb_pos.to_uint64(),
                              dmrs.config_type,
                              dmrs.dmrs_scrambling_id,
                              dmrs.dmrs_scrambling_id_complement,
                              dmrs.low_papr_dmrs ? fapi::low_papr_dmrs_type::dependent_cdm_group
                                                 : fapi::low_papr_dmrs_type::independent_cdm_group,
                              dmrs.n_scid,
                              dmrs.num_dmrs_cdm_grps_no_data,
                              dmrs.dmrs_ports.to_uint64());
}

static void fill_frequency_allocation(fapi::dl_pdsch_pdu_builder&   builder,
                                      const prb_grant&              prbs,
                                      fapi::vrb_to_prb_mapping_type mapping)
{
  if (prbs.is_alloc_type0()) {
    static_vector<uint8_t, fapi::dl_pdsch_pdu::MAX_SIZE_RB_BITMAP> rb_map;
    rb_map.resize(fapi::dl_pdsch_pdu::MAX_SIZE_RB_BITMAP, 0U);
    const rbg_bitmap& mac_rbg_map = prbs.rbgs();
    for (unsigned i = 0, e = mac_rbg_map.size(); i != e; ++i) {
      rb_map[i / 8] |= uint8_t(mac_rbg_map.test(i) ? 1U : 0U) << i % 8;
    }
    builder.set_pdsch_allocation_in_frequency_type_0({rb_map}, mapping);
  } else {
    const prb_interval& prb_int = prbs.prbs();
    builder.set_pdsch_allocation_in_frequency_type_1(prb_int.start(), prb_int.length(), mapping);
  }
}

static void fill_time_allocation(fapi::dl_pdsch_pdu_builder& builder, const ofdm_symbol_range& symbols)
{
  builder.set_pdsch_allocation_in_time_parameters(symbols.start(), symbols.length());
}

static void fill_power_parameters(fapi::dl_pdsch_pdu_builder& builder)
{
  builder.set_tx_power_info_parameters({0}, fapi::nzp_csi_rs_epre_to_ssb::dB0);

  // Default v3 powers.
  builder.set_maintenance_v3_tx_power_info_parameters({}, {});
}

static void fill_pdsch_information(fapi::dl_pdsch_pdu_builder& builder, const pdsch_information& pdsch_cfg)
{
  // Basic parameters.
  builder.set_basic_parameters(pdsch_cfg.rnti);

  // Codewords.
  fill_codewords(builder, pdsch_cfg.codewords);

  // DMRS.
  fill_dmrs(builder, pdsch_cfg.dmrs);

  // Time allocation.
  fill_time_allocation(builder, pdsch_cfg.symbols);
}

static void fill_coreset(fapi::dl_pdsch_pdu_builder&  builder,
                         const coreset_configuration& coreset_cfg,
                         const bwp_configuration&     bwp_cfg,
                         fapi::pdsch_trans_type       trans_type,
                         bool                         is_coreset0_config_for_cell)
{
  unsigned coreset_start_point = 0;
  unsigned initial_dl_bwp_size = 0;

  switch (trans_type) {
    case srsgnb::fapi::pdsch_trans_type::non_interleaved_common_ss:
    case srsgnb::fapi::pdsch_trans_type::interleaved_common_any_coreset0_present:
    case srsgnb::fapi::pdsch_trans_type::interleaved_common_any_coreset0_not_present:
      coreset_start_point = coreset_cfg.get_coreset_start_crb();
      break;
    default:
      break;
  }

  switch (trans_type) {
    case srsgnb::fapi::pdsch_trans_type::interleaved_common_any_coreset0_present:
    case srsgnb::fapi::pdsch_trans_type::interleaved_common_any_coreset0_not_present:
      initial_dl_bwp_size =
          (is_coreset0_config_for_cell) ? coreset_cfg.coreset0_crbs().length() : bwp_cfg.crbs.length();
      break;
    default:
      break;
  }

  builder.set_maintenance_v3_bwp_parameters(trans_type, coreset_start_point, initial_dl_bwp_size);
}

static fapi::pdsch_trans_type get_pdsch_trans_type(coreset_id            id,
                                                   search_space_set_type ss,
                                                   bool                  is_interleaved,
                                                   bool                  is_dci_1_0,
                                                   bool                  is_coreset0_config_for_cell)
{
  // Non-interleaved cases.
  if (!is_interleaved) {
    if (is_dci_1_0 && is_common_search_space(ss)) {
      return fapi::pdsch_trans_type::non_interleaved_common_ss;
    }
    return fapi::pdsch_trans_type::non_interleaved_other;
  }

  // Interleaved cases for DCI 1_0, CORESET0 and in Common Search Space.
  if (id == to_coreset_id(0) && is_common_search_space(ss) && is_dci_1_0) {
    if (ss == srsgnb::search_space_set_type::type0) {
      return fapi::pdsch_trans_type::interleaved_common_type0_coreset0;
    }

    return is_coreset0_config_for_cell ? fapi::pdsch_trans_type::interleaved_common_any_coreset0_present
                                       : fapi::pdsch_trans_type::interleaved_common_any_coreset0_not_present;
  }

  return fapi::pdsch_trans_type::interleaved_other;
}

void srsgnb::fapi_adaptor::convert_pdsch_mac_to_fapi(fapi::dl_pdsch_pdu_builder& builder,
                                                     const sib_information&      mac_pdu)
{
  srsgnb_assert(mac_pdu.pdsch_cfg.codewords.size() == 1, "This version only supports one transport block");
  srsgnb_assert(mac_pdu.pdsch_cfg.coreset_cfg, "Invalid CORESET configuration");

  // Fill all the parameters contained in the MAC PDSCH information struct.
  fill_pdsch_information(builder, mac_pdu.pdsch_cfg);

  // Codeword information.
  fill_codeword_information(builder, mac_pdu.pdsch_cfg.n_id, fapi::pdsch_ref_point_type::subcarrier_0);

  // BWP parameters.
  const crb_interval& crbs = get_crb_interval(mac_pdu.pdsch_cfg);
  builder.set_bwp_parameters(crbs.length(),
                             crbs.start(),
                             mac_pdu.pdsch_cfg.bwp_cfg->scs,
                             mac_pdu.pdsch_cfg.bwp_cfg->cp_extended ? cyclic_prefix::EXTENDED : cyclic_prefix::NORMAL);

  fill_power_parameters(builder);

  // Get the VRB-to-PRB mapping from the DCI.
  bool is_interleaved = mac_pdu.pdsch_cfg.is_interleaved;
  // Frequency allocation.
  // Note: As defined in TS38.214 Section 5.1.2.3, DCI format 1_0 uses bundle size of 2.
  fill_frequency_allocation(builder,
                            mac_pdu.pdsch_cfg.prbs,
                            is_interleaved ? fapi::vrb_to_prb_mapping_type::interleaved_rb_size2
                                           : fapi::vrb_to_prb_mapping_type::non_interleaved);

  fapi::pdsch_trans_type trans_type = get_pdsch_trans_type(mac_pdu.pdsch_cfg.coreset_cfg->id,
                                                           mac_pdu.pdsch_cfg.ss_set_type,
                                                           is_interleaved,
                                                           mac_pdu.pdsch_cfg.dci_fmt == dci_dl_format::f1_0,
                                                           is_coreset0_configured_for_cell);

  fill_coreset(
      builder, *mac_pdu.pdsch_cfg.coreset_cfg, *mac_pdu.pdsch_cfg.bwp_cfg, trans_type, is_coreset0_configured_for_cell);

  // :TODO Rate-Matching related parameters, not used now.
}

void srsgnb::fapi_adaptor::convert_pdsch_mac_to_fapi(fapi::dl_pdsch_pdu& fapi_pdu, const rar_information& mac_pdu)
{
  fapi::dl_pdsch_pdu_builder builder(fapi_pdu);

  convert_pdsch_mac_to_fapi(builder, mac_pdu);
}

void srsgnb::fapi_adaptor::convert_pdsch_mac_to_fapi(fapi::dl_pdsch_pdu_builder& builder,
                                                     const rar_information&      mac_pdu)
{
  srsgnb_assert(mac_pdu.pdsch_cfg.codewords.size() == 1, "This version only supports one transport block");
  srsgnb_assert(mac_pdu.pdsch_cfg.coreset_cfg, "Invalid CORESET configuration");

  // Fill all the parameters contained in the MAC PDSCH information struct.
  fill_pdsch_information(builder, mac_pdu.pdsch_cfg);

  // Codeword information.
  fill_codeword_information(builder, mac_pdu.pdsch_cfg.n_id, fapi::pdsch_ref_point_type::point_a);

  // BWP parameters.
  const crb_interval& crbs = get_crb_interval(mac_pdu.pdsch_cfg);
  builder.set_bwp_parameters(crbs.length(),
                             crbs.start(),
                             mac_pdu.pdsch_cfg.bwp_cfg->scs,
                             mac_pdu.pdsch_cfg.bwp_cfg->cp_extended ? cyclic_prefix::EXTENDED : cyclic_prefix::NORMAL);

  fill_power_parameters(builder);

  // Get the VRB-to-PRB mapping from the DCI.
  bool is_interleaved = mac_pdu.pdsch_cfg.is_interleaved;
  // Frequency allocation.
  // Note: As defined in TS38.214 Section 5.1.2.3, DCI format 1_0 uses bundle size of 2.
  fill_frequency_allocation(builder,
                            mac_pdu.pdsch_cfg.prbs,
                            is_interleaved ? fapi::vrb_to_prb_mapping_type::interleaved_rb_size2
                                           : fapi::vrb_to_prb_mapping_type::non_interleaved);

  fapi::pdsch_trans_type trans_type = get_pdsch_trans_type(mac_pdu.pdsch_cfg.coreset_cfg->id,
                                                           mac_pdu.pdsch_cfg.ss_set_type,
                                                           is_interleaved,
                                                           mac_pdu.pdsch_cfg.dci_fmt == dci_dl_format::f1_0,
                                                           is_coreset0_configured_for_cell);

  fill_coreset(
      builder, *mac_pdu.pdsch_cfg.coreset_cfg, *mac_pdu.pdsch_cfg.bwp_cfg, trans_type, is_coreset0_configured_for_cell);

  // :TODO Rate-Matching related parameters, not used now.
}

void srsgnb::fapi_adaptor::convert_pdsch_mac_to_fapi(fapi::dl_pdsch_pdu& fapi_pdu, const dl_msg_alloc& mac_pdu)
{
  fapi::dl_pdsch_pdu_builder builder(fapi_pdu);

  convert_pdsch_mac_to_fapi(builder, mac_pdu);
}

void srsgnb::fapi_adaptor::convert_pdsch_mac_to_fapi(fapi::dl_pdsch_pdu_builder& builder, const dl_msg_alloc& mac_pdu)
{
  srsgnb_assert(mac_pdu.pdsch_cfg.codewords.size() == 1, "This version only supports one transport block");
  srsgnb_assert(mac_pdu.pdsch_cfg.coreset_cfg, "Invalid CORESET configuration");

  // Fill all the parameters contained in the MAC PDSCH information struct.
  fill_pdsch_information(builder, mac_pdu.pdsch_cfg);

  // Codeword information.
  fill_codeword_information(builder, mac_pdu.pdsch_cfg.n_id, fapi::pdsch_ref_point_type::point_a);

  // BWP parameters.
  const crb_interval& crbs = get_crb_interval(mac_pdu.pdsch_cfg);
  builder.set_bwp_parameters(crbs.length(),
                             crbs.start(),
                             mac_pdu.pdsch_cfg.bwp_cfg->scs,
                             mac_pdu.pdsch_cfg.bwp_cfg->cp_extended ? cyclic_prefix::EXTENDED : cyclic_prefix::NORMAL);

  fill_power_parameters(builder);

  // Get the VRB-to-PRB mapping from the DCI.
  bool is_interleaved = mac_pdu.pdsch_cfg.is_interleaved;
  // Frequency allocation.
  // Note: As defined in TS38.214 Section 5.1.2.3, DCI format 1_0 uses bundle size of 2.
  fill_frequency_allocation(builder,
                            mac_pdu.pdsch_cfg.prbs,
                            is_interleaved ? fapi::vrb_to_prb_mapping_type::interleaved_rb_size2
                                           : fapi::vrb_to_prb_mapping_type::non_interleaved);

  fapi::pdsch_trans_type trans_type = get_pdsch_trans_type(mac_pdu.pdsch_cfg.coreset_cfg->id,
                                                           mac_pdu.pdsch_cfg.ss_set_type,
                                                           is_interleaved,
                                                           mac_pdu.pdsch_cfg.dci_fmt == dci_dl_format::f1_0,
                                                           is_coreset0_configured_for_cell);

  fill_coreset(
      builder, *mac_pdu.pdsch_cfg.coreset_cfg, *mac_pdu.pdsch_cfg.bwp_cfg, trans_type, is_coreset0_configured_for_cell);

  // :TODO Rate-Matching related parameters, not used now.
}
