/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "../support/rb_helper.h"
#include "srsran/adt/expected.h"
#include "srsran/scheduler/scheduler_configurator.h"

namespace srsran {

/// Holds the configuration of a cell.
/// Additionally, this class pre-caches the computation of some const values related to the cell configuration
/// and provide parameter getter helpers.
class cell_configuration
{
public:
  explicit cell_configuration(const sched_cell_configuration_request_message& msg);
  cell_configuration(const cell_configuration&) = delete;
  cell_configuration(cell_configuration&&)      = delete;

  const du_cell_index_t                   cell_index;
  const du_cell_group_index_t             cell_group_index;
  const pci_t                             pci;
  const unsigned                          nof_dl_prbs;
  const unsigned                          nof_ul_prbs;
  const unsigned                          nof_slots_per_frame;
  const dl_config_common                  dl_cfg_common;
  const ul_config_common                  ul_cfg_common;
  const optional<tdd_ul_dl_config_common> tdd_cfg_common;

  /// Imported from mac_cell_configuration.
  carrier_configuration   dl_carrier;
  const ssb_configuration ssb_cfg;
  dmrs_typeA_position     dmrs_typeA_pos;
  carrier_configuration   ul_carrier;

  /// List of PUCCH guardbands.
  const std::vector<sched_grid_resource> pucch_guardbands;

  /// List of zp-CSI-RS resources.
  std::vector<zp_csi_rs_resource> zp_csi_rs_list;

  /// List of nzp-CSI-RS resources.
  std::vector<nzp_csi_rs_resource> nzp_csi_rs_list;

  // Derived Parameters.
  ssb_pattern_case ssb_case;
  bool             paired_spectrum;
  const nr_band    band;
  uint8_t          L_max;

  bool is_tdd() const { return tdd_cfg_common.has_value(); }

  /// Checks if DL/UL is active for current slot
  bool is_fully_dl_enabled(slot_point sl) const
  {
    if (dl_symbols_per_slot_lst.empty()) {
      // Note: dl_enabled_slot_lst is empty in the FDD case.
      return true;
    }
    if (sl.numerology() != to_numerology_value(tdd_cfg_common->ref_scs)) {
      // Convert slot into equivalent reference SCS.
      sl = set_slot_numerology(sl, to_numerology_value(tdd_cfg_common->ref_scs));
    }
    return dl_symbols_per_slot_lst[sl.to_uint() % dl_symbols_per_slot_lst.size()] ==
           get_nsymb_per_slot(dl_cfg_common.init_dl_bwp.generic_params.cp);
  }
  bool is_fully_ul_enabled(slot_point sl) const
  {
    if (ul_symbols_per_slot_lst.empty()) {
      // Note: ul_enabled_slot_lst is empty in the FDD case.
      return true;
    }
    if (sl.numerology() != to_numerology_value(tdd_cfg_common->ref_scs)) {
      // Convert slot into equivalent reference SCS.
      sl = set_slot_numerology(sl, to_numerology_value(tdd_cfg_common->ref_scs));
    }
    return ul_symbols_per_slot_lst[sl.to_uint() % ul_symbols_per_slot_lst.size()] ==
           get_nsymb_per_slot(ul_cfg_common.init_ul_bwp.generic_params.cp);
  }

  bool is_dl_enabled(slot_point sl) const
  {
    if (dl_symbols_per_slot_lst.empty()) {
      // Note: dl_enabled_slot_lst is empty in the FDD case.
      return true;
    }
    if (sl.numerology() != to_numerology_value(tdd_cfg_common->ref_scs)) {
      // Convert slot into equivalent reference SCS.
      sl = set_slot_numerology(sl, to_numerology_value(tdd_cfg_common->ref_scs));
    }
    return dl_symbols_per_slot_lst[sl.to_uint() % dl_symbols_per_slot_lst.size()] > 0;
  }
  bool is_ul_enabled(slot_point sl) const
  {
    if (ul_symbols_per_slot_lst.empty()) {
      // Note: ul_enabled_slot_lst is empty in the FDD case.
      return true;
    }
    if (sl.numerology() != to_numerology_value(tdd_cfg_common->ref_scs)) {
      // Convert slot into equivalent reference SCS.
      sl = set_slot_numerology(sl, to_numerology_value(tdd_cfg_common->ref_scs));
    }
    return ul_symbols_per_slot_lst[sl.to_uint() % ul_symbols_per_slot_lst.size()] > 0;
  }
  unsigned get_nof_dl_symbol_per_slot(slot_point sl) const
  {
    if (dl_symbols_per_slot_lst.empty()) {
      // Note: dl_enabled_slot_lst is empty in the FDD case.
      return get_nsymb_per_slot(dl_cfg_common.init_dl_bwp.generic_params.cp);
    }
    if (sl.numerology() != to_numerology_value(tdd_cfg_common->ref_scs)) {
      // Convert slot into equivalent reference SCS.
      sl = set_slot_numerology(sl, to_numerology_value(tdd_cfg_common->ref_scs));
    }
    return dl_symbols_per_slot_lst[sl.to_uint() % dl_symbols_per_slot_lst.size()];
  }
  unsigned get_nof_ul_symbol_per_slot(slot_point sl) const
  {
    if (ul_symbols_per_slot_lst.empty()) {
      // Note: ul_enabled_slot_lst is empty in the FDD case.
      return get_nsymb_per_slot(ul_cfg_common.init_ul_bwp.generic_params.cp);
    }
    if (sl.numerology() != to_numerology_value(tdd_cfg_common->ref_scs)) {
      // Convert slot into equivalent reference SCS.
      sl = set_slot_numerology(sl, to_numerology_value(tdd_cfg_common->ref_scs));
    }
    return ul_symbols_per_slot_lst[sl.to_uint() % ul_symbols_per_slot_lst.size()];
  }
  const coreset_configuration& get_common_coreset(coreset_id cs_id) const
  {
    // The existence of the Coreset (either CommonCoreset or Coreset0) has been verified by the validator.
    return dl_cfg_common.init_dl_bwp.pdcch_common.common_coreset.has_value() and
                   dl_cfg_common.init_dl_bwp.pdcch_common.common_coreset.value().id == cs_id
               ? dl_cfg_common.init_dl_bwp.pdcch_common.common_coreset.value()
               : dl_cfg_common.init_dl_bwp.pdcch_common.coreset0.value();
  }

private:
  /// Vector circularly indexed by slot with the list of nof active DL/UL symbols per slot.
  std::vector<unsigned> dl_symbols_per_slot_lst;
  std::vector<unsigned> ul_symbols_per_slot_lst;
};

} // namespace srsran
