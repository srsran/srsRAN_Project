/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pbch_processor_impl.h"
#include "srsgnb/support/math_utils.h"

using namespace srsgnb;

void pbch_processor_impl::proccess(const srsgnb::pbch_processor::pdu_t& pdu, srsgnb::resource_grid_writer& grid)
{
  // Calculate derivative parameters
  unsigned l_start_in_burst = ssb_get_l_first(pdu.pattern_case, pdu.ssb_idx);
  unsigned l_start          = (l_start_in_burst % NSYMB_PER_SLOT_NORM);
  unsigned k_start          = ssb_get_k_first(pdu.slot.numerology, pdu.ssb_offset_pointA, pdu.ssb_subcarrier_offset);

  // Make sure the slot matches with the SS/PBCH transmission slot
  assert((l_start_in_burst / NSYMB_PER_SLOT_NORM) == pdu.slot.get_half_frame_slot());

  // Generate PBCH message
  pbch_encoder::pbch_msg_t pbch_msg = {};
  pbch_msg.N_id                     = pdu.phys_cell_id;
  pbch_msg.ssb_idx                  = pdu.ssb_idx;
  pbch_msg.L_max                    = pdu.L_max;
  pbch_msg.hrf                      = pdu.slot.get_half_radio_frame();
  pbch_msg.payload                  = pdu.bch_payload;
  pbch_msg.sfn                      = pdu.slot.frame;
  pbch_msg.k_ssb                    = pdu.ssb_subcarrier_offset;

  // Encode PBCH
  std::array<uint8_t, pbch_encoder::E> encoded_bits;
  encoder->encode(pbch_msg, encoded_bits);

  // Create modulator configuration
  pbch_modulator::config_t modulator_config = {};
  modulator_config.phys_cell_id             = pdu.phys_cell_id;
  modulator_config.ssb_idx                  = pdu.ssb_idx;
  modulator_config.ssb_first_subcarrier     = k_start;
  modulator_config.ssb_first_symbol         = l_start;
  modulator_config.amplitude                = 1.0F;

  // Modulate PBCH
  modulator->put(encoded_bits, grid, modulator_config);

  // Create DMRS for PBCH configuration
  dmrs_pbch_processor::config_t dmrs_config = {};
  dmrs_config.phys_cell_id                  = pdu.phys_cell_id;
  dmrs_config.ssb_idx                       = pdu.ssb_idx;
  dmrs_config.L_max                         = pdu.L_max;
  dmrs_config.ssb_first_subcarrier          = k_start;
  dmrs_config.ssb_first_symbol              = l_start;
  dmrs_config.n_hf                          = pdu.slot.get_half_radio_frame();
  dmrs_config.amplitude                     = 1.0F;

  // Put DMRS for PBCH
  dmrs->map(grid, dmrs_config);

  // Create PSS configuration
  pss_processor::config_t pss_config = {};
  pss_config.phys_cell_id            = pdu.phys_cell_id;
  pss_config.ssb_first_symbol        = l_start;
  pss_config.ssb_first_subcarrier    = k_start;
  pss_config.amplitude               = convert_dB_to_amplitude(pdu.beta_pss);

  // Put PSS
  pss->map(grid, pss_config);

  // Create SSS configuration
  sss_processor::config_t sss_config = {};
  sss_config.phys_cell_id            = pdu.phys_cell_id;
  sss_config.ssb_first_symbol        = l_start;
  sss_config.ssb_first_subcarrier    = k_start;
  sss_config.amplitude               = 1.0F;

  // Put SSS
  sss->map(grid, sss_config);
}

std::unique_ptr<pbch_processor> srsgnb::create_pbch_processor(pbch_processor_config& config)
{
  return std::make_unique<pbch_processor_impl>(config);
}