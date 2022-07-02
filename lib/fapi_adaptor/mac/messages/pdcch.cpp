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
using namespace fapi;
using namespace fapi_adaptor;

void srsgnb::fapi_adaptor::convert_pdcch_mac_to_fapi(fapi::dl_pdcch_pdu& fapi_pdu, const mac_pdcch_pdu& mac_pdu)
{
  dl_pdcch_pdu_builder builder(fapi_pdu);
  convert_pdcch_mac_to_fapi(builder, mac_pdu);
}

void srsgnb::fapi_adaptor::convert_pdcch_mac_to_fapi(fapi::dl_pdcch_pdu_builder& builder, const mac_pdcch_pdu& mac_pdu)
{
  const std::vector<dci_info>& dcis = mac_pdu.dcis;
  srsran_assert(!dcis.empty(), "No DL_DCI to add to the PDCCH PDU");

  const bwp_configuration& bwp_cfg = *mac_pdu.bwp_cfg;
  // Fill BWP parameters.
  builder.set_bwp_parameters(bwp_cfg.crbs.length(),
                             bwp_cfg.crbs.start(),
                             bwp_cfg.scs,
                             (bwp_cfg.cp_extended) ? cyclic_prefix_type::extended : cyclic_prefix_type::normal);

  // Fill Coreset parameters.
  const coreset_configuration& coreset_cfg = *mac_pdu.coreset_cfg;
  // :TODO: change the start symbol index in the future.
  unsigned start_symbol_index = 0;
  builder.set_coreset_parameters(
      start_symbol_index,
      coreset_cfg.duration,
      coreset_cfg.freq_domain_resources,
      coreset_cfg.interleaved.has_value() ? cce_to_reg_mapping_type::interleaved
                                          : cce_to_reg_mapping_type::non_interleaved,
      coreset_cfg.interleaved.has_value() ? coreset_cfg.interleaved.value().reg_bundle_sz : 6U,
      coreset_cfg.interleaved.has_value() ? coreset_cfg.interleaved.value().interleaver_sz : 0U,
      // :TODO :check this parameter.
      (dcis.front().parameters->dci.type == dci_dl_rnti_config_type::si_f1_0) ? pdcch_coreset_type::pbch_or_sib1
                                                                              : pdcch_coreset_type::other,
      coreset_cfg.interleaved.has_value() ? coreset_cfg.interleaved.value().shift_index.has_value()
                                                ? coreset_cfg.interleaved.value().shift_index.value()
                                                : 0U
                                          : 0U,
      coreset_cfg.precoder_granurality);

  // Fill the DCIs.
  for (const auto& dci : dcis) {
    dl_dci_pdu_builder dci_builder = builder.add_dl_dci();

    dci_builder.set_basic_parameters(dci.parameters->ctx.rnti,
                                     dci.parameters->ctx.n_id_pdcch_data,
                                     dci.parameters->ctx.n_rnti_pdcch_data,
                                     dci.parameters->ctx.cces.ncce,
                                     to_nof_cces(dci.parameters->ctx.cces.aggr_lvl));

    // :TODO: check this power value.
    dci_builder.set_tx_power_info_parameter({0.F});

    // :TODO: check collocated Al16 candicate and powers.
    dci_builder.set_maintenance_v3_dci_parameters(false, {}, {});

    // :TODO: Is this correct? In FAPI this is a DCI parameter.
    dci_builder.set_parameters_v4_dci(
        coreset_cfg.pdcch_dmrs_scrambling_id.has_value() ? coreset_cfg.pdcch_dmrs_scrambling_id.value() : 0U);

    dci_builder.set_payload(*dci.payload);
  }
}
