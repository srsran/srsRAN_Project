
#ifndef SRSGNB_MAC_MAC_H
#define SRSGNB_MAC_MAC_H

#include "mac_cell_control_information_handler.h"
#include "mac_cell_slot_handler.h"
#include "mac_manager.h"
#include "mac_pdu_handler.h"
#include "mac_rach_handler.h"
#include "mac_ue_configurator.h"
#include <memory>

namespace srsgnb {

class mac_interface
{
public:
  virtual ~mac_interface() = default;

  /// Returns handler of PRACHs.
  virtual mac_rach_handler& get_rach_handler() = 0;

  /// Returns handler for UL and DL control information.
  virtual mac_control_information_handler& get_control_info_handler() = 0;

  /// Returns handler of slot indications for a cell with provided cell_index.
  virtual mac_cell_slot_handler& get_slot_handler(du_cell_index_t cell_index) = 0;

  /// Returns MAC management handler.
  virtual mac_manager& get_manager() = 0;

  /// Returns MAC UE configuration handler.
  virtual mac_ue_configurator& get_ue_configurator() = 0;

  /// Returns SDU handler.
  virtual mac_pdu_handler& get_pdu_handler() = 0;
};

} // namespace srsgnb

#endif // SRSGNB_MAC_MAC_H
