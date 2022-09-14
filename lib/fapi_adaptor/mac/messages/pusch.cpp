/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/fapi_adaptor/mac/messages/pusch.h"
#include "srsgnb/mac/mac_cell_result.h"
#include "srsgnb/phy/upper/channel_coding/ldpc/ldpc.h"

using namespace srsgnb;
using namespace fapi_adaptor;

void srsgnb::fapi_adaptor::convert_pusch_mac_to_fapi(fapi::ul_pusch_pdu& fapi_pdu, const pusch_information& mac_pdu)
{
  fapi::ul_pusch_pdu_builder builder(fapi_pdu);

  convert_pusch_mac_to_fapi(builder, mac_pdu);
}

static fapi::dmrs_config_type convert_dmrs_type_mac_to_fapi(srsgnb::dmrs_config_type type)
{
  switch (type) {
    case srsgnb::dmrs_config_type::type1:
      return fapi::dmrs_config_type::type_1;
    case srsgnb::dmrs_config_type::type2:
      return fapi::dmrs_config_type::type_2;
    default:
      srsgnb_assert(0, "Unexpected DMRS type ({})", static_cast<unsigned>(type));
      break;
  }
  // Fallback value.
  return fapi::dmrs_config_type::type_1;
}

void srsgnb::fapi_adaptor::convert_pusch_mac_to_fapi(fapi::ul_pusch_pdu_builder& builder,
                                                     const pusch_information&    mac_pdu)
{
  // :TODO: chech this handle. It will be better to pass it from the translator, as the adaptor doesn't know how many
  // PDUs are expected.
  unsigned handle = 0;
  builder.set_basic_parameters(mac_pdu.rnti, handle);

  const bwp_configuration& bwp_cfg = *mac_pdu.bwp_cfg;
  builder.set_bwp_parameters(bwp_cfg.crbs.length(),
                             bwp_cfg.crbs.start(),
                             bwp_cfg.scs,
                             (bwp_cfg.cp_extended) ? fapi::cyclic_prefix_type::extended
                                                   : fapi::cyclic_prefix_type::normal);

  float R = mac_pdu.target_code_rate * (1.F / 1024);
  builder.set_information_parameters(R,
                                     mac_pdu.qam_mod,
                                     mac_pdu.mcs_index.to_uint(),
                                     mac_pdu.mcs_table,
                                     mac_pdu.transform_precoding,
                                     mac_pdu.n_id,
                                     mac_pdu.nof_layers);

  const dmrs_information& dmrs_cfg = mac_pdu.dmrs;
  builder.set_dmrs_parameters(dmrs_cfg.dmrs_symb_pos.to_uint64(),
                              convert_dmrs_type_mac_to_fapi(dmrs_cfg.config_type),
                              dmrs_cfg.dmrs_scrambling_id,
                              dmrs_cfg.dmrs_scrambling_id_complement,
                              dmrs_cfg.low_papr_dmrs ? fapi::low_papr_dmrs_type::dependent_cdm_group
                                                     : fapi::low_papr_dmrs_type::independent_cdm_group,
                              mac_pdu.pusch_dmrs_id,
                              dmrs_cfg.n_scid,
                              dmrs_cfg.num_dmrs_cdm_grps_no_data,
                              dmrs_cfg.dmrs_ports.to_uint64());

  const prb_grant& prbs = mac_pdu.prbs;
  if (prbs.is_alloc_type0()) {
    static_vector<uint8_t, fapi::ul_pusch_pdu::RB_BITMAP_SIZE_IN_BYTES> rb_map;
    rb_map.resize(fapi::ul_pusch_pdu::RB_BITMAP_SIZE_IN_BYTES, 0U);
    const rbg_bitmap& mac_rbg_map = prbs.rbgs();
    for (unsigned i = 0, e = mac_rbg_map.size(); i != e; ++i) {
      rb_map[i / 8] |= uint8_t(mac_rbg_map.test(i) ? 1U : 0U) << i % 8;
    }

    builder.set_allocation_in_frequency_type_0_parameters(
        {rb_map}, mac_pdu.tx_direct_current_location, mac_pdu.ul_freq_shift_7p5khz);
  } else {
    const prb_interval& prb_int = prbs.prbs();
    builder.set_allocation_in_frequency_type_1_parameters(prb_int.start(),
                                                          prb_int.length(),
                                                          mac_pdu.intra_slot_freq_hopping,
                                                          mac_pdu.tx_direct_current_location,
                                                          mac_pdu.ul_freq_shift_7p5khz);
  }

  builder.set_allocation_in_time_parameters(mac_pdu.symbols.start(), mac_pdu.symbols.length());

  // Add PUSCH Data.
  builder.add_optional_pusch_data(
      mac_pdu.rv_index, mac_pdu.harq_id, mac_pdu.new_data, mac_pdu.tb_size_bytes, mac_pdu.num_cb, {});

  unsigned tb_size_lbrm_bytes = ldpc::MAX_CODEBLOCK_SIZE / 8;
  builder.set_maintenance_v3_frequency_allocation_parameters(
      mac_pdu.pusch_second_hop_prb, get_ldpc_base_graph(R, mac_pdu.tb_size_bytes), tb_size_lbrm_bytes);

  builder.set_maintenance_v3_dmrs_parameters(static_cast<unsigned>(mac_pdu.dmrs_hopping_mode));

  // :TODO: add the rest of the parameters.
  // :TODO: add optional UCI.
}
