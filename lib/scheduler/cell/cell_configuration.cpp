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

using namespace srsran;

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
  pucch_guardbands(msg.pucch_guardbands),
  csi_meas_cfg(msg.csi_meas_cfg),
  // SSB derived params.
  ssb_case(ssb_get_ssb_pattern(msg.ssb_config.scs, msg.dl_carrier.arfcn)),
  paired_spectrum(band_helper::is_paired_spectrum(band_helper::get_band_from_dl_arfcn(msg.dl_carrier.arfcn))),
  L_max(ssb_get_L_max(msg.ssb_config.scs, msg.dl_carrier.arfcn))
{
  if (tdd_cfg_common.has_value()) {
    // Cache list of DL and UL slots in case of TDD
    unsigned tdd_period_slots = nof_slots_per_tdd_period(*msg.tdd_ul_dl_cfg_common);
    dl_enabled_slot_lst.resize(tdd_period_slots);
    ul_enabled_slot_lst.resize(tdd_period_slots);
    for (unsigned slot_period_idx = 0; slot_period_idx < dl_enabled_slot_lst.size(); ++slot_period_idx) {
      dl_enabled_slot_lst[slot_period_idx] = has_active_tdd_dl_symbols(*msg.tdd_ul_dl_cfg_common, slot_period_idx);
      ul_enabled_slot_lst[slot_period_idx] = has_active_tdd_ul_symbols(*msg.tdd_ul_dl_cfg_common, slot_period_idx);
    }
  }
}
