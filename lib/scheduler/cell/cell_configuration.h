/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef LIB_MAC_SCHED_CELL_CONFIGURATION_H
#define LIB_MAC_SCHED_CELL_CONFIGURATION_H

#include "srsgnb/adt/expected.h"
#include "srsgnb/scheduler/scheduler_configurator.h"

namespace srsgnb {

/// Calculates number of slots, using TDD reference SCS, of the TDD UL-DL configuration.
unsigned nof_slots_per_tdd_period(const tdd_ul_dl_config_common& cfg);

/// Checks whether provided slot corresponds to a DL slot.
bool slot_is_dl(const tdd_ul_dl_config_common& cfg, slot_point slot);

/// Checks whether provided slot corresponds to an UL slot.
bool slot_is_ul(const tdd_ul_dl_config_common& cfg, slot_point slot);

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

  // Derived Parameters.
  ssb_pattern_case ssb_case;
  bool             paired_spectrum;
  uint8_t          L_max;

  bool is_tdd() const { return tdd_cfg_common.has_value(); }

  /// Checks if DL/UL is active for current slot
  bool is_dl_enabled(slot_point sl) const
  {
    // Note: dl_enabled_slot_lst is empty in the FDD case.
    return dl_enabled_slot_lst.empty() or
           static_cast<bool>(dl_enabled_slot_lst[sl.to_uint() % dl_enabled_slot_lst.size()]);
  }
  bool is_ul_enabled(slot_point sl) const
  {
    // Note: ul_enabled_slot_lst is empty in the FDD case.
    return ul_enabled_slot_lst.empty() or
           static_cast<bool>(ul_enabled_slot_lst[sl.to_uint() % ul_enabled_slot_lst.size()]);
  }

private:
  /// Vector circularly indexed by slot that indicates whether a slot has DL/UL enabled.
  /// Note: I use uint8_t to avoid vector<bool> special case.
  std::vector<uint8_t> dl_enabled_slot_lst;
  std::vector<uint8_t> ul_enabled_slot_lst;
};

/// Verify correctness of cell configuration request message
error_type<std::string> is_cell_configuration_request_valid(const sched_cell_configuration_request_message& msg);

} // namespace srsgnb

#endif // LIB_MAC_SCHED_CELL_CONFIGURATION_H
