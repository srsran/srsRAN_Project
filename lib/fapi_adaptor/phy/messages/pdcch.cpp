/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/fapi_adaptor/phy/messages/pdcch.h"
#include "srsgnb/srsvec/bit.h"

using namespace srsgnb;
using namespace fapi_adaptor;
using namespace fapi;

/// Fills the DL DCI parameters of the PDCCH processor pdu.
static void fill_dci(pdcch_processor::pdu_t& proc_pdu, const dl_pdcch_pdu& fapi_pdu)
{
  for (unsigned i = 0, e = fapi_pdu.dl_dci.size(); i != e; ++i) {
    proc_pdu.dci_list.emplace_back();
    auto&       dci         = proc_pdu.dci_list.back();
    const auto& fapi_dci    = fapi_pdu.dl_dci[i];
    const auto& fapi_dci_v3 = fapi_pdu.maintenance_v3.info[i];
    const auto& fapi_dci_v4 = fapi_pdu.parameters_v4.params[i];

    dci.n_id_pdcch_data   = fapi_dci.nid_pdcch_data;
    dci.n_id_pdcch_dmrs   = fapi_dci_v4.nid_pdcch_dmrs;
    dci.n_rnti            = fapi_dci.nrnti_pdcch_data;
    dci.cce_index         = fapi_dci.cce_index;
    dci.aggregation_level = fapi_dci.aggregation_level;

    dci.dmrs_power_offset_dB = (fapi_dci.power_control_offset_ss_profile_nr == -127)
                                   ? static_cast<float>(fapi_dci_v3.pdcch_dmrs_power_offset_profile_sss) * 0.001F
                                   : static_cast<float>(fapi_dci.power_control_offset_ss_profile_nr);

    dci.data_power_offset_dB = (fapi_dci_v3.pdcch_data_power_offset_profile_sss == std::numeric_limits<int16_t>::min())
                                   ? dci.dmrs_power_offset_dB
                                   : float(fapi_dci_v3.pdcch_data_power_offset_profile_sss) * 0.001F;

    // Unpack the payload.
    // :TODO: Verify in the future that the payload should be unpacked.
    dci.payload.resize(fapi_dci.payload.size() * 8);
    srsvec::bit_unpack({dci.payload}, {fapi_dci.payload});

    // :TODO: Fill this in the future.
    dci.ports = {0};
  }
}

/// Fills the coreset parameters of the PDCCH processor pdu.
static void fill_coreset(pdcch_processor::coreset_description& coreset, const dl_pdcch_pdu& fapi_pdu)
{
  coreset.bwp_size_rb             = fapi_pdu.coreset_bwp_size;
  coreset.bwp_start_rb            = fapi_pdu.coreset_bwp_start;
  coreset.start_symbol_index      = fapi_pdu.start_symbol_index;
  coreset.duration                = fapi_pdu.duration_symbols;
  coreset.cce_to_reg_mapping_type = (fapi_pdu.cce_reg_mapping_type == cce_to_reg_mapping_type::non_interleaved)
                                        ? pdcch_processor::coreset_description::NON_INTERLEAVED
                                        : pdcch_processor::coreset_description::INTERLEAVED;

  coreset.reg_bundle_size      = fapi_pdu.reg_bundle_size;
  coreset.interleaver_size     = fapi_pdu.interleaver_size;
  coreset.type                 = (fapi_pdu.coreset_type == pdcch_coreset_type::pbch_or_sib1)
                                     ? pdcch_processor::coreset_description::CORESET0
                                     : pdcch_processor::coreset_description::OTHER;
  coreset.shift_index          = fapi_pdu.shift_index;
  coreset.precoder_granularity = (fapi_pdu.precoder_granularity == precoder_granularity_type::same_as_reg_bundle)
                                     ? pdcch_processor::coreset_description::SAME_AS_REG_BUNDLE
                                     : pdcch_processor::coreset_description::ALL_CONTIGUOUS_RBS;

  // :TODO: frequency_resources. Verify how this field is codified to do the transformation correctly, and how the phy
  // expects the order.
  for (unsigned i = 0, e = 45; i != e; ++i) {
    unsigned byte = i / 8;
    unsigned pos  = i % 8;

    coreset.frequency_resources[i] = (static_cast<unsigned>(fapi_pdu.freq_domain_resource[byte] >> pos) & 1U) == 1U;
  }
}

void srsgnb::fapi_adaptor::convert_pdcch_fapi_to_phy(pdcch_processor::pdu_t& proc_pdu,
                                                     const dl_pdcch_pdu&     fapi_pdu,
                                                     uint16_t                sfn,
                                                     uint16_t                slot)
{
  proc_pdu.slot = slot_point(static_cast<uint32_t>(fapi_pdu.scs), sfn, slot);
  proc_pdu.cp =
      cyclic_prefix((fapi_pdu.cyclic_prefix == cyclic_prefix_type::normal) ? cyclic_prefix::options::NORMAL
                                                                           : cyclic_prefix::options::EXTENDED);

  fill_coreset(proc_pdu.coreset, fapi_pdu);

  fill_dci(proc_pdu, fapi_pdu);
}
