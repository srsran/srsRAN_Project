/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

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
  const pci_t                             pci;
  const unsigned                          nof_dl_prbs;
  const unsigned                          nof_ul_prbs;
  const unsigned                          nof_slots_per_frame;
  const dl_config_common                  dl_cfg_common;
  const ul_config_common                  ul_cfg_common;
  const optional<tdd_ul_dl_config_common> tdd_cfg_common;

  /// Imported from mac_cell_configuration.
  /// For dl_carrier, only arfcn is currently used.
  carrier_configuration   dl_carrier;
  const ssb_configuration ssb_cfg;
  dmrs_typeA_position     dmrs_typeA_pos;

  /// List of PUCCH guardbands;
  const std::vector<sched_grid_resource> pucch_guardbands;

  /// CSI-RS scheduling parameters.
  optional<csi_meas_config> csi_meas_cfg;

  // Derived Parameters.
  ssb_pattern_case ssb_case;
  bool             paired_spectrum;
  uint8_t          L_max;

  bool is_tdd() const { return tdd_cfg_common.has_value(); }

  /// Checks if DL/UL is active for current slot
  bool is_dl_enabled(slot_point sl) const
  {
    if (dl_enabled_slot_lst.empty()) {
      // Note: dl_enabled_slot_lst is empty in the FDD case.
      return true;
    }
    if (sl.numerology() != to_numerology_value(tdd_cfg_common->ref_scs)) {
      // Convert slot into equivalent reference SCS.
      sl = set_slot_numerology(sl, to_numerology_value(tdd_cfg_common->ref_scs));
    }
    return dl_enabled_slot_lst[sl.to_uint() % dl_enabled_slot_lst.size()];
  }
  bool is_ul_enabled(slot_point sl) const
  {
    if (ul_enabled_slot_lst.empty()) {
      // Note: ul_enabled_slot_lst is empty in the FDD case.
      return true;
    }
    if (sl.numerology() != to_numerology_value(tdd_cfg_common->ref_scs)) {
      // Convert slot into equivalent reference SCS.
      sl = set_slot_numerology(sl, to_numerology_value(tdd_cfg_common->ref_scs));
    }
    return ul_enabled_slot_lst[sl.to_uint() % ul_enabled_slot_lst.size()];
  }

private:
  /// Vector circularly indexed by slot that indicates whether a slot has DL/UL enabled.
  /// Note: I use uint8_t to avoid vector<bool> special case.
  std::vector<uint8_t> dl_enabled_slot_lst;
  std::vector<uint8_t> ul_enabled_slot_lst;
};

} // namespace srsran
