
#ifndef SRSGNB_MAC_CELL_CONFIGURATION_H
#define SRSGNB_MAC_CELL_CONFIGURATION_H

#include "srsgnb/adt/expected.h"
#include "srsgnb/mac/sched_configurer.h"

namespace srsgnb {

/// Holds the configuration of a cell and provides some getter helpers
class cell_configuration
{
public:
  explicit cell_configuration(const cell_configuration_request_message& msg);
  cell_configuration(const cell_configuration&) = delete;
  cell_configuration(cell_configuration&&)      = delete;

  const bool                                           is_tdd;
  const du_cell_index_t                                cell_index;
  const pci_t                                          pci;
  const unsigned                                       nof_dl_prbs;
  const unsigned                                       nof_ul_prbs;
  const asn1::rrc_nr::dl_cfg_common_sib_s              dl_cfg_common;
  const asn1::rrc_nr::ul_cfg_common_sib_s              ul_cfg_common;
  const optional<asn1::rrc_nr::tdd_ul_dl_cfg_common_s> tdd_cfg_common;

  const asn1::rrc_nr::subcarrier_spacing_opts scs() const
  {
    return dl_cfg_common.init_dl_bwp.generic_params.subcarrier_spacing;
  }
};

/// Verify correctness of cell configuration request message
error_type<std::string> is_cell_configuration_request_valid(const cell_configuration_request_message& msg);

} // namespace srsgnb

#endif // SRSGNB_MAC_CELL_CONFIGURATION_H
