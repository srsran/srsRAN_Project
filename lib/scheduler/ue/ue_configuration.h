
#ifndef SRSGNB_UE_CONFIGURATION_H
#define SRSGNB_UE_CONFIGURATION_H

#include "srsgnb/ran/bwp_configuration.h"
#include "srsgnb/ran/du_types.h"

namespace srsgnb {

class ue_carrier_configuration
{
public:
  /// Find Common or UE-dedicated BWP configuration.
  const bwp_configuration*          get_bwp_cfg(du_bwp_id_t bwp_id) const { return nullptr; }
  const coreset_configuration*      get_cs_cfg(du_bwp_id_t bwp_id, coreset_id cs_id) const { return nullptr; }
  const search_space_configuration* get_ss_cfg(du_bwp_id_t bwp_id, search_space_id ss_id) const { return nullptr; }

private:
  slot_vector<bwp_downlink_common> dl_bwps;
};

} // namespace srsgnb

#endif // SRSGNB_UE_CONFIGURATION_H
