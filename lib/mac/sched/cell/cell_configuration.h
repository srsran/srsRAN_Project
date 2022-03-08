
#ifndef SRSGNB_MAC_CELL_CONFIGURATION_H
#define SRSGNB_MAC_CELL_CONFIGURATION_H

#include "srsgnb/adt/expected.h"
#include "srsgnb/mac/sched_interface.h"

namespace srsgnb {

/// Holds the configuration of a cell and provides some getter helpers
class cell_configuration
{
public:
  explicit cell_configuration(const cell_configuration_request_message& msg);
  cell_configuration(const cell_configuration&) = delete;
  cell_configuration(cell_configuration&&)      = delete;

  const du_cell_index_t cell_index;
  const pci_t           pci;
  const duplex_mode_t   duplex_mode;
  const unsigned        nof_dl_prbs;
  const unsigned        nof_ul_prbs;
};

/// Verify correctness of cell configuration request message
error_type<std::string> is_cell_configuration_request_valid(const cell_configuration_request_message& msg);

} // namespace srsgnb

#endif // SRSGNB_MAC_CELL_CONFIGURATION_H
