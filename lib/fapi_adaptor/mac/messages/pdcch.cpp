/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/fapi_adaptor/mac/messages/pdcch.h"

using namespace srsgnb;
using namespace fapi_adaptor;

void srsgnb::fapi_adaptor::convert_pdcch_mac_to_fapi(fapi::dl_pdcch_pdu& fapi_pdu, const mac_pdcch_pdu& mac_pdu)
{
  fapi::dl_pdcch_pdu_builder builder(fapi_pdu);
  convert_pdcch_mac_to_fapi(builder, mac_pdu);
}

static void fill_bwp_parameters(fapi::dl_pdcch_pdu_builder&  builder,
                                const bwp_configuration&     bwp_cfg,
                                const coreset_configuration& coreset_cfg)
{
  // According to the FreqDomainResource description field in the FAPI specs, for CORESET0 we need to take its starting
  // point and size, otherwise, for the rest of CORESETs take it from the BWP where the CORESET belongs.
  const crb_interval& crbs = (coreset_cfg.id == to_coreset_id(0)) ? coreset_cfg.coreset0_crbs() : bwp_cfg.crbs;

  builder.set_bwp_parameters(crbs.length(),
                             crbs.start(),
                             bwp_cfg.scs,
                             (bwp_cfg.cp_extended) ? cyclic_prefix::EXTENDED : cyclic_prefix::NORMAL);
}

static freq_resource_bitmap calculate_coreset0_freq_res_bitmap(const coreset_configuration& coreset_cfg)
{
  freq_resource_bitmap freq_bitmap_coreset0(pdcch_constants::MAX_NOF_FREQ_RESOURCES);

  unsigned num_bits = coreset_cfg.coreset0_crbs().length() / pdcch_constants::NOF_RB_PER_FREQ_RESOURCE;
  freq_bitmap_coreset0.fill(0, num_bits);

  return freq_bitmap_coreset0;
}

static void fill_coreset_parameters(fapi::dl_pdcch_pdu_builder&  builder,
                                    const coreset_configuration& coreset_cfg,
                                    unsigned                     start_symbol_index)
{
  builder.set_coreset_parameters(
      start_symbol_index,
      coreset_cfg.duration,
      (coreset_cfg.id == to_coreset_id(0)) ? calculate_coreset0_freq_res_bitmap(coreset_cfg)
                                           : coreset_cfg.freq_domain_resources(),
      coreset_cfg.interleaved.has_value() ? fapi::cce_to_reg_mapping_type::interleaved
                                          : fapi::cce_to_reg_mapping_type::non_interleaved,
      coreset_cfg.interleaved.has_value() ? coreset_cfg.interleaved.value().reg_bundle_sz : 6U,
      coreset_cfg.interleaved.has_value() ? coreset_cfg.interleaved.value().interleaver_sz : 0U,
      (coreset_cfg.id == to_coreset_id(0)) ? fapi::pdcch_coreset_type::pbch_or_coreset0
                                           : fapi::pdcch_coreset_type::other,
      coreset_cfg.interleaved.has_value() ? coreset_cfg.interleaved.value().shift_index : 0U,
      coreset_cfg.precoder_granurality);
}

void srsgnb::fapi_adaptor::convert_pdcch_mac_to_fapi(fapi::dl_pdcch_pdu_builder& builder, const mac_pdcch_pdu& mac_pdu)
{
  const static_vector<mac_pdcch_pdu::dci_info, fapi::MAX_NUM_DCIS_PER_PDCCH_PDU>& dcis = mac_pdu.dcis;
  srsgnb_assert(!dcis.empty(), "No DCIs to add into the PDCCH PDU");

  const coreset_configuration& coreset_cfg = *mac_pdu.coreset_cfg;

  // Fill BWP parameters.
  fill_bwp_parameters(builder, *mac_pdu.bwp_cfg, coreset_cfg);

  // Fill CORESET parameters.
  fill_coreset_parameters(builder, *mac_pdu.coreset_cfg, mac_pdu.start_symbol);

  // Fill the DCIs.
  for (const auto& dci : dcis) {
    fapi::dl_dci_pdu_builder dci_builder = builder.add_dl_dci();

    dci_builder.set_basic_parameters(dci.parameters->ctx.rnti,
                                     dci.parameters->ctx.n_id_pdcch_data,
                                     dci.parameters->ctx.n_rnti_pdcch_data,
                                     dci.parameters->ctx.cces.ncce,
                                     to_nof_cces(dci.parameters->ctx.cces.aggr_lvl));

    // This parameter is not passed by the MAC, set it to zero.
    dci_builder.set_tx_power_info_parameter(0.F);

    // These parameters are not passed by the MAC, leave them as disabled.
    dci_builder.set_maintenance_v3_dci_parameters(false, {}, {});

    dci_builder.set_parameters_v4_dci(
        coreset_cfg.pdcch_dmrs_scrambling_id.has_value() ? coreset_cfg.pdcch_dmrs_scrambling_id.value() : 0);

    dci_builder.set_payload(*dci.payload);
  }
}
