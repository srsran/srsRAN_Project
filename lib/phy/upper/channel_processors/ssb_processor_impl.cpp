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

#include "ssb_processor_impl.h"
#include "srsgnb/phy/cyclic_prefix.h"
#include "srsgnb/support/math_utils.h"

using namespace srsgnb;

void ssb_processor_impl::process(const pdu_t& pdu, resource_grid_writer& grid)
{
  // Calculate derivative parameters
  unsigned l_start_in_burst = ssb_get_l_first(pdu.pattern_case, pdu.ssb_idx);
  unsigned l_start          = (l_start_in_burst % get_nsymb_per_slot(cyclic_prefix::NORMAL));
  unsigned k_start          = ssb_get_k_first(pdu.slot.numerology(), pdu.ssb_offset_pointA, pdu.ssb_subcarrier_offset);

  // Make sure the slot matches with the SS/PBCH transmission slot
  srsran_assert((l_start_in_burst / get_nsymb_per_slot(cyclic_prefix::NORMAL)) == pdu.slot.hrf_slot_index(),
                "Invalid slot index ({}) for SSB index {}",
                pdu.slot.hrf_slot_index(),
                l_start_in_burst);

  // Generate PBCH message
  pbch_encoder::pbch_msg_t pbch_msg = {};
  pbch_msg.N_id                     = pdu.phys_cell_id;
  pbch_msg.ssb_idx                  = pdu.ssb_idx;
  pbch_msg.L_max                    = pdu.L_max;
  pbch_msg.hrf                      = pdu.slot.is_odd_hrf();
  pbch_msg.payload                  = pdu.bch_payload;
  pbch_msg.sfn                      = pdu.slot.sfn();
  pbch_msg.k_ssb                    = pdu.ssb_subcarrier_offset;

  // Encode PBCH
  std::array<uint8_t, pbch_encoder::E> encoded_bits;
  encoder->encode(encoded_bits, pbch_msg);

  // Create modulator configuration
  pbch_modulator::config_t modulator_config = {};
  modulator_config.phys_cell_id             = pdu.phys_cell_id;
  modulator_config.ssb_idx                  = pdu.ssb_idx;
  modulator_config.ssb_first_subcarrier     = k_start;
  modulator_config.ssb_first_symbol         = l_start;
  modulator_config.amplitude                = 1.0F;
  modulator_config.ports                    = pdu.ports;

  // Modulate PBCH
  modulator->put(encoded_bits, grid, modulator_config);

  // Create DMRS for PBCH configuration
  dmrs_pbch_processor::config_t dmrs_config = {};
  dmrs_config.phys_cell_id                  = pdu.phys_cell_id;
  dmrs_config.ssb_idx                       = pdu.ssb_idx;
  dmrs_config.L_max                         = pdu.L_max;
  dmrs_config.ssb_first_subcarrier          = k_start;
  dmrs_config.ssb_first_symbol              = l_start;
  dmrs_config.n_hf                          = pdu.slot.is_odd_hrf();
  dmrs_config.amplitude                     = 1.0F;
  dmrs_config.ports                         = pdu.ports;

  // Put DMRS for PBCH
  dmrs->map(grid, dmrs_config);

  // Create PSS configuration
  pss_processor::config_t pss_config = {};
  pss_config.phys_cell_id            = pdu.phys_cell_id;
  pss_config.ssb_first_symbol        = l_start;
  pss_config.ssb_first_subcarrier    = k_start;
  pss_config.amplitude               = convert_dB_to_amplitude(pdu.beta_pss);
  pss_config.ports                   = pdu.ports;

  // Put PSS
  pss->map(grid, pss_config);

  // Create SSS configuration
  sss_processor::config_t sss_config = {};
  sss_config.phys_cell_id            = pdu.phys_cell_id;
  sss_config.ssb_first_symbol        = l_start;
  sss_config.ssb_first_subcarrier    = k_start;
  sss_config.amplitude               = 1.0F;
  sss_config.ports                   = pdu.ports;

  // Put SSS
  sss->map(grid, sss_config);
}

std::unique_ptr<ssb_processor> srsgnb::create_ssb_processor(ssb_processor_config& config)
{
  return std::make_unique<ssb_processor_impl>(config);
}
