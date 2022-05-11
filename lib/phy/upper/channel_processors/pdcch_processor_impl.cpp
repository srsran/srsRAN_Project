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

#include "pdcch_processor_impl.h"
#include "srsgnb/support/math_utils.h"

using namespace srsgnb;
using namespace pdcch_constants;

void pdcch_processor_impl::map_reg_to_prb(span<bool>                 rb_mask,
                                          span<const bool>           reg_mask,
                                          const coreset_description& coreset)
{
  // For CORESET 0, ignores the frequency resource list.
  if (coreset.type == coreset_description::CORESET0) {
    // Copy the REG mask directly on RB.
    std::copy(reg_mask.begin(), reg_mask.end(), rb_mask.begin() + coreset.bwp_start_rb);

    // Set the rest of RB to false.
    std::fill(rb_mask.begin(), rb_mask.begin() + coreset.bwp_start_rb, false);
    std::fill(rb_mask.begin() + coreset.bwp_start_rb + reg_mask.size(), rb_mask.end(), false);

    return;
  }

  // Initialize RB mask to all false.
  std::fill(rb_mask.begin(), rb_mask.end(), false);

  // For each available frequency resource map REG into RB.
  for (unsigned freq_res_idx = 0, coreset_rb_idx = 0; freq_res_idx != MAX_NOF_FREQ_RESOUCES; ++freq_res_idx) {
    // Skip if not available.
    if (!coreset.frequency_resources[freq_res_idx]) {
      continue;
    }

    // Iterate each resource block in the frequency resource.
    unsigned rb_start = freq_res_idx * NOF_RB_PER_FREQ_RESOURCE;
    unsigned rb_end   = (freq_res_idx + 1) * NOF_RB_PER_FREQ_RESOURCE;
    for (unsigned rb_idx = rb_start; rb_idx != rb_end; ++rb_idx, ++coreset_rb_idx) {
      rb_mask[rb_idx + coreset.bwp_start_rb] = reg_mask[coreset_rb_idx];
    }
  }
}

void pdcch_processor_impl::compute_rb_mask_non_interleaved(span<bool>                 mask,
                                                           const coreset_description& coreset,
                                                           const dci_description&     dci)
{
  // Determine the number of CCE.
  unsigned nof_cce = dci.aggregation_level;

  // Determine the CCE range.
  unsigned cce_start = dci.cce_index;
  unsigned cce_end   = dci.cce_index + nof_cce;

  // Convert from CCE to REG.
  unsigned reg_start = cce_start * NOF_REG_PER_CCE / coreset.duration;
  unsigned reg_end   = cce_end * NOF_REG_PER_CCE / coreset.duration;

  // Create REG mask.
  static_vector<bool, MAX_RB> reg_mask(coreset.bwp_size_rb);

  // Fill active REG.
  std::fill(reg_mask.begin() + reg_start, reg_mask.begin() + reg_end, true);

  // Fill inactive REG.
  std::fill(reg_mask.begin(), reg_mask.begin() + reg_start, false);
  std::fill(reg_mask.begin() + reg_end, reg_mask.end(), false);

  // Map REG into RB.
  map_reg_to_prb(mask, reg_mask, coreset);
}

void pdcch_processor_impl::compute_rb_mask_interleaved(span<bool>                 mask,
                                                       const coreset_description& coreset,
                                                       const dci_description&     dci)
{
  // Count the total amount of RB enabled for the given CORESET.
  unsigned coreset_bw_rb = coreset.bwp_size_rb;
  if (coreset.type != coreset_description::CORESET0) {
    coreset_bw_rb = 0;
    for (unsigned freq_res_idx = 0; freq_res_idx != MAX_NOF_FREQ_RESOUCES; ++freq_res_idx) {
      if (coreset.frequency_resources[freq_res_idx]) {
        coreset_bw_rb += NOF_RB_PER_FREQ_RESOURCE;
      }
    }
  }

  // Load interleaver parameters.
  unsigned L = coreset.reg_bundle_size;
  unsigned R = coreset.interleaver_size;

  // Override interleaver parameters for CORESET0.
  if (coreset.type == coreset_description::CORESET0) {
    L = 6;
    R = 2;
  }

  // Calculate CORESET constants.
  unsigned N_CORESET_REG = coreset.duration * coreset_bw_rb;
  unsigned C             = N_CORESET_REG / (L * R);
  unsigned n_shift       = coreset.shift_index;

  // Validate parameters.
  srsran_assert(N_CORESET_REG > 0 && (N_CORESET_REG % (L * R) == 0) && (L % coreset.duration == 0),
                "Invalid CORESET configuration.");

  unsigned nof_cce        = dci.aggregation_level;
  unsigned nof_reg_bundle = 6 / L;

  // Create REG mask.
  static_vector<bool, MAX_RB> reg_mask(coreset_bw_rb);

  // For each CCE index j in the PDCCH transmission...
  unsigned cce_start = dci.cce_index;
  unsigned cce_end   = dci.cce_index + nof_cce;
  for (unsigned j = cce_start; j != cce_end; ++j) {
    // For each REG bundle i in the CCE j...
    for (unsigned reg_bundle_idx = 0; reg_bundle_idx < nof_reg_bundle; ++reg_bundle_idx) {
      // Calculate x variable.
      unsigned x = (6 * j) / L + reg_bundle_idx;

      // For non interleaved f(x) = x.
      unsigned r = x % R;
      unsigned c = x / R;
      unsigned i = (r * C + c + n_shift) % (N_CORESET_REG / L);

      // For each REG in the REG bundle.
      unsigned reg_start = (i * L) / coreset.duration;
      unsigned reg_end   = ((i + 1) * L) / coreset.duration;
      for (unsigned reg_idx = reg_start; reg_idx != reg_end; ++reg_idx) {
        reg_mask[reg_idx] = true;
      }
    }
  }

  // Map REG into RB.
  map_reg_to_prb(mask, reg_mask, coreset);
}

void pdcch_processor_impl::compute_rb_mask(span<bool>                 mask,
                                           const coreset_description& coreset,
                                           const dci_description&     dci)
{
  // Interleaved case.
  if (coreset.type == coreset_description::CORESET0 ||
      coreset.cce_to_reg_mapping_type == coreset_description::INTERLEAVED) {
    compute_rb_mask_interleaved(mask, coreset, dci);
    return;
  }

  // Non-interleaved case.
  compute_rb_mask_non_interleaved(mask, coreset, dci);
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
    static_vector<bool, MAX_RB> rb_mask(coreset.bwp_size_rb);
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
    dmrs_pdcch_config.reference_point_k_rb = coreset.type == coreset_description::CORESET0 ? coreset.bwp_start_rb : 0;
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
