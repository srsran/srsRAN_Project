
#ifndef LIB_MAC_SCHED_CELL_CONFIGURATION_H
#define LIB_MAC_SCHED_CELL_CONFIGURATION_H

#include "srsgnb/adt/expected.h"
#include "srsgnb/mac/sched_configurator.h"

namespace srsgnb {

/// Holds the configuration of a cell.
/// Additionally, this class pre-caches the computation of some const values related to the cell configuration
/// and provide parameter getter helpers.
class cell_configuration
{
public:
  explicit cell_configuration(const cell_configuration_request_message& msg);
  cell_configuration(const cell_configuration&) = delete;
  cell_configuration(cell_configuration&&)      = delete;

  const du_cell_index_t                                cell_index;
  const pci_t                                          pci;
  const unsigned                                       nof_dl_prbs;
  const unsigned                                       nof_ul_prbs;
  const unsigned                                       nof_slots_per_frame;
  const asn1::rrc_nr::dl_cfg_common_sib_s              dl_cfg_common;
  const asn1::rrc_nr::ul_cfg_common_sib_s              ul_cfg_common;
  const optional<asn1::rrc_nr::tdd_ul_dl_cfg_common_s> tdd_cfg_common;

  bool is_tdd() const { return tdd_cfg_common.has_value(); }

  /// DL Subcarrier Spacing
  asn1::rrc_nr::subcarrier_spacing_e scs() const { return dl_cfg_common.init_dl_bwp.generic_params.subcarrier_spacing; }

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
           not static_cast<bool>(ul_enabled_slot_lst[sl.to_uint() % ul_enabled_slot_lst.size()]);
  }

private:
  /// Vector circularly indexed by slot that indicates whether a slot has DL/UL enabled.
  /// Note: I use uint8_t to avoid vector<bool> special case.
  std::vector<uint8_t> dl_enabled_slot_lst;
  std::vector<uint8_t> ul_enabled_slot_lst;
};

/// Verify correctness of cell configuration request message
error_type<std::string> is_cell_configuration_request_valid(const cell_configuration_request_message& msg);

} // namespace srsgnb

#endif // LIB_MAC_SCHED_CELL_CONFIGURATION_H
