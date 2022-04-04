
#ifndef SRSGNB_DCI_ENCODER_H
#define SRSGNB_DCI_ENCODER_H

#include "srsgnb/mac/mac_cell_result.h"
#include "srsgnb/mac/mac_resource_manager.h"
#include "srsgnb/mac/sched_result.h"

namespace srsgnb {

/// Encodes DCI of SIB allocation.
inline void encode_dci(const mac_cell_configuration& cell_cfg, const sib_information& sib, mac_dl_dci_payload& dci)
{
  // TODO
}

/// Encodes DCI of RAR allocation.
inline void encode_dci(const mac_cell_configuration& cell_cfg, const rar_information& rar, mac_dl_dci_payload& dci)
{
  // TODO
}

/// Encodes DCI of UE grant allocation.
inline void encode_dci(const mac_cell_configuration& cell_cfg, const dl_msg_alloc& msg, mac_dl_dci_payload& dci)
{
  // TODO
}

} // namespace srsgnb

#endif // SRSGNB_DCI_ENCODER_H
