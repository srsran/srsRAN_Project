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
  switch (coreset.cce_to_reg_mapping) {
    case cce_to_reg_mapping_type::CORESET0:
      prb_indexes = cce_to_prb_mapping_coreset0(coreset.bwp_start_rb,
                                                coreset.bwp_size_rb,
                                                coreset.duration,
                                                coreset.shift_index,
                                                dci.aggregation_level,
                                                dci.cce_index);
      break;
    case cce_to_reg_mapping_type::NON_INTERLEAVED:
      prb_indexes = cce_to_prb_mapping_non_interleaved(
          coreset.bwp_start_rb, coreset.frequency_resources, coreset.duration, dci.aggregation_level, dci.cce_index);
      break;
    case cce_to_reg_mapping_type::INTERLEAVED:
      prb_indexes = cce_to_prb_mapping_interleaved(coreset.bwp_start_rb,
                                                   coreset.frequency_resources,
                                                   coreset.duration,
                                                   coreset.reg_bundle_size,
                                                   coreset.interleaver_size,
                                                   coreset.shift_index,
                                                   dci.aggregation_level,
                                                   dci.cce_index);
      break;
  }

  bounded_bitset<MAX_RB> result(coreset.bwp_start_rb + coreset.bwp_size_rb);
  for (uint16_t prb_index : prb_indexes) {
    result.set(prb_index, true);
  }
  return result;
}

void pdcch_processor_impl::process(srsgnb::resource_grid_writer& grid, const pdcch_processor::pdu_t& pdu)
{
  const coreset_description& coreset = pdu.coreset;

  // Verify CORESET.
  srsgnb_assert(coreset.duration > 0 && coreset.duration <= MAX_CORESET_DURATION,
                "Invalid CORESET duration (%d)",
                coreset.duration);

  // For each DCI described in the PDU...
  for (const dci_description& dci : pdu.dci_list) {
    // Generate RB mask.
    bounded_bitset<MAX_RB> rb_mask = compute_rb_mask(coreset, dci);

    // Populate PDCCH encoder configuration.
    pdcch_encoder::config_t encoder_config = {};
    encoder_config.E                       = dci.aggregation_level * NOF_REG_PER_CCE * NOF_RE_PDCCH_PER_RB * 2;
    encoder_config.rnti                    = dci.rnti;

    // Encode.
    span<uint8_t> encoded = span<uint8_t>(temp_encoded).first(nof_encoded_bits(dci.aggregation_level));
    encoder->encode(encoded, dci.payload, encoder_config);

    // Populate PDCCH modulator configuration.
    pdcch_modulator::config_t modulator_config = {};
    modulator_config.rb_mask                   = rb_mask;
    modulator_config.start_symbol_index        = coreset.start_symbol_index;
    modulator_config.duration                  = coreset.duration;
    modulator_config.n_id                      = dci.n_id_pdcch_data;
    modulator_config.n_rnti                    = dci.n_rnti;
    modulator_config.scaling                   = convert_dB_to_amplitude(dci.data_power_offset_dB);
    modulator_config.ports                     = dci.ports;

    // Modulate.
    modulator->modulate(grid, encoded, modulator_config);

    unsigned reference_point_k_rb =
        coreset.cce_to_reg_mapping == cce_to_reg_mapping_type::CORESET0 ? coreset.bwp_start_rb : 0;

    // Populate DMRS for PDCCH configuration.
    dmrs_pdcch_processor::config_t dmrs_pdcch_config = {};
    dmrs_pdcch_config.slot                           = pdu.slot;
    dmrs_pdcch_config.cp                             = pdu.cp;
    dmrs_pdcch_config.reference_point_k_rb           = reference_point_k_rb;
    dmrs_pdcch_config.rb_mask                        = rb_mask;
    dmrs_pdcch_config.start_symbol_index             = coreset.start_symbol_index;
    dmrs_pdcch_config.duration                       = coreset.duration;
    dmrs_pdcch_config.n_id                           = dci.n_id_pdcch_dmrs;
    dmrs_pdcch_config.amplitude                      = convert_dB_to_amplitude(dci.dmrs_power_offset_dB);
    dmrs_pdcch_config.ports                          = dci.ports;

    // Generate DMRS.
    dmrs->map(grid, dmrs_pdcch_config);
  }
}
