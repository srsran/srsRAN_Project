/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cell_configuration.h"
#include "srsgnb/ran/band_helper.h"
#include "srsgnb/ran/resource_block.h"
#include "srsgnb/ran/ssb_mapping.h"

using namespace srsgnb;

unsigned srsgnb::nof_slots_per_tdd_period(const tdd_ul_dl_config_common& cfg)
{
  return cfg.pattern1.dl_ul_tx_period_nof_slots +
         (cfg.pattern2.has_value() ? cfg.pattern2->dl_ul_tx_period_nof_slots : 0U);
}

bool srsgnb::slot_is_dl(const tdd_ul_dl_config_common& cfg, slot_point slot)
{
  unsigned period_slots = nof_slots_per_tdd_period(cfg);

  // Convert slot into reference SCS.
  if (slot.numerology() != to_numerology_value(cfg.ref_scs)) {
    slot = set_slot_numerology(slot, to_numerology_value(cfg.ref_scs));
  }

  // Calculate slot index within the TDD overall period
  unsigned slot_idx_period = slot.to_uint() % period_slots; // Slot index within the period

  // Select pattern
  const tdd_ul_dl_pattern* pattern = &cfg.pattern1;
  if (slot_idx_period >= cfg.pattern1.dl_ul_tx_period_nof_slots) {
    pattern = &cfg.pattern2.value();
    slot_idx_period -= cfg.pattern1.dl_ul_tx_period_nof_slots; // Remove pattern 1 offset
  }

  // Check DL boundaries. Both fully DL slots and partially DL slots return true.
  return (slot_idx_period < pattern->nof_dl_slots ||
          (slot_idx_period == pattern->nof_dl_slots && pattern->nof_dl_symbols != 0));
}

bool srsgnb::slot_is_ul(const tdd_ul_dl_config_common& cfg, slot_point slot)
{
  // TODO: Implement it properly
  return not slot_is_dl(cfg, slot);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

cell_configuration::cell_configuration(const sched_cell_configuration_request_message& msg) :
  cell_index(msg.cell_index),
  pci(msg.pci),
  nof_dl_prbs(get_max_Nprb(msg.dl_carrier.carrier_bw_mhz, msg.scs_common, frequency_range::FR1)),
  nof_ul_prbs(get_max_Nprb(msg.ul_carrier.carrier_bw_mhz, msg.scs_common, frequency_range::FR1)),
  nof_slots_per_frame(get_nof_slots_per_subframe(msg.dl_cfg_common.init_dl_bwp.generic_params.scs)),
  dl_cfg_common(msg.dl_cfg_common),
  ul_cfg_common(msg.ul_cfg_common),
  tdd_cfg_common(msg.tdd_ul_dl_cfg_common),
  dl_carrier(msg.dl_carrier),
  ssb_cfg(msg.ssb_config),
  dmrs_typeA_pos(msg.dmrs_typeA_pos),
  // SSB derived params.
  ssb_case(ssb_get_ssb_pattern(msg.ssb_config.scs, msg.dl_carrier.arfcn)),
  paired_spectrum(band_helper::is_paired_spectrum(band_helper::get_band_from_dl_arfcn(msg.dl_carrier.arfcn))),
  L_max(ssb_get_L_max(msg.ssb_config.scs, msg.dl_carrier.arfcn))
{
  if (tdd_cfg_common.has_value()) {
    // Cache list of DL and UL slots in case of TDD
    uint8_t numerology_idx = to_numerology_value(msg.tdd_ul_dl_cfg_common->ref_scs);
    dl_enabled_slot_lst.resize(nof_slots_per_tdd_period(*msg.tdd_ul_dl_cfg_common));
    ul_enabled_slot_lst.resize(nof_slots_per_tdd_period(*msg.tdd_ul_dl_cfg_common));
    for (unsigned i = 0; i < dl_enabled_slot_lst.size(); ++i) {
      dl_enabled_slot_lst[i] = (uint8_t)slot_is_dl(*msg.tdd_ul_dl_cfg_common, slot_point{numerology_idx, i});
      ul_enabled_slot_lst[i] = (uint8_t)slot_is_ul(*msg.tdd_ul_dl_cfg_common, slot_point{numerology_idx, i});
    }
  }
}
