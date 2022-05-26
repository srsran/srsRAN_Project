/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pdcch_processor_impl.h"
#include "srsgnb/ran/pdcch/cce_to_prb_mapping.h"
#include "srsgnb/support/math_utils.h"

using namespace srsgnb;
using namespace pdcch_constants;

bounded_bitset<MAX_RB> pdcch_processor_impl::compute_rb_mask(const coreset_description& coreset,
                                                             const dci_description&     dci)
{
  prb_index_list prb_indexes;
  if (coreset.cce_to_reg_mapping_type == coreset_description::CORESET0) {
    prb_indexes = cce_to_prb_mapping_coreset0(coreset.bwp_start_rb,
                                              coreset.bwp_size_rb,
                                              coreset.duration,
                                              coreset.shift_index,
                                              dci.aggregation_level,
                                              dci.cce_index);
  } else {
    if (coreset.cce_to_reg_mapping_type != coreset_description::INTERLEAVED) {
      prb_indexes = cce_to_prb_mapping_non_interleaved(
          coreset.bwp_start_rb, coreset.frequency_resources, coreset.duration, dci.aggregation_level, dci.cce_index);
    } else {
      prb_indexes = cce_to_prb_mapping_interleaved(coreset.bwp_start_rb,
                                                   coreset.frequency_resources,
                                                   coreset.duration,
                                                   coreset.reg_bundle_size,
                                                   coreset.interleaver_size,
                                                   coreset.shift_index,
                                                   dci.aggregation_level,
                                                   dci.cce_index);
    }
  }

  bounded_bitset<MAX_RB> result(coreset.bwp_start_rb + coreset.bwp_size_rb);
  for (uint16_t prb_index : prb_indexes) {
    result.set(prb_index, true);
  }
  return result;
}

void pdcch_processor_impl::process(srsgnb::resource_grid_writer& grid, srsgnb::pdcch_processor::pdu_t& pdu)
{
  const coreset_description& coreset = pdu.coreset;

  // Verify CORESET.
  srsran_assert(coreset.duration > 0 && coreset.duration <= MAX_CORESET_DURATION,
                "Invalid CORESET duration (%d)",
                coreset.duration);

  // For each DCI described in the PDU...
  for (const dci_description& dci : pdu.dci_list) {
    // Generate RB mask.
    static_vector<bool, MAX_RB> rb_mask(coreset.bwp_start_rb + coreset.bwp_size_rb);
    compute_rb_mask(rb_mask, coreset, dci);

    // Extract payload.
    const static_vector<uint8_t, MAX_DCI_PAYLOAD_SIZE>& payload = dci.payload;

    // Populate PDCCH encoder configuration.
    pdcch_encoder::config_t encoder_config = {};

    // Encode.
    static_vector<uint8_t, MAX_NOF_BITS> encoded(nof_encoded_bits(dci.aggregation_level));
    encoder->encode(encoded, payload, encoder_config);

    // Populate PDCCH modulator configuration.
    pdcch_modulator::config_t modulator_config = {};
    std::copy(rb_mask.begin(), rb_mask.end(), modulator_config.rb_mask.begin());
    modulator_config.start_symbol_index = coreset.start_symbol_index;
    modulator_config.duration           = coreset.duration;
    modulator_config.n_id               = dci.n_id_pdcch_data;
    modulator_config.n_rnti             = dci.n_rnti;
    modulator_config.scaling            = convert_dB_to_amplitude(dci.data_power_offset_dB);
    modulator_config.ports              = dci.ports;

    // Modulate.
    modulator->modulate(grid, encoded, modulator_config);

    // Populate DMRS for PDCCH configuration.
    dmrs_pdcch_processor::config_t dmrs_pdcch_config = {};
    dmrs_pdcch_config.slot                           = pdu.slot;
    dmrs_pdcch_config.cp                             = pdu.cp;
    dmrs_pdcch_config.reference_point_k_rb =
        coreset.cce_to_reg_mapping_type == coreset_description::CORESET0 ? coreset.bwp_start_rb : 0;
    std::copy(rb_mask.begin(), rb_mask.end(), dmrs_pdcch_config.rb_mask.begin());
    dmrs_pdcch_config.start_symbol_index = coreset.start_symbol_index;
    dmrs_pdcch_config.duration           = coreset.duration;
    dmrs_pdcch_config.n_id               = dci.n_id_pdcch_dmrs;
    dmrs_pdcch_config.amplitude          = convert_dB_to_amplitude(dci.dmrs_power_offset_dB);
    dmrs_pdcch_config.ports              = dci.ports;

    // Generate DMRS.
    dmrs->map(grid, dmrs_pdcch_config);
  }
}

std::unique_ptr<pdcch_processor> srsgnb::create_pdcch_processor(pdcch_processor_config_t& config)
{
  return std::make_unique<pdcch_processor_impl>(config);
}
