/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_DCI_ENCODER_H
#define SRSGNB_DCI_ENCODER_H

#include "srsgnb/mac/mac_cell_manager.h"
#include "srsgnb/mac/mac_cell_result.h"

namespace srsgnb {

/// Encodes DCI of SIB allocation.
inline void encode_dci(const mac_cell_creation_request& cell_cfg, const sib_information& sib, mac_dl_dci_payload& dci)
{
  // TODO
}

/// Encodes DCI of RAR allocation.
inline void encode_dci(const mac_cell_creation_request& cell_cfg, const rar_information& rar, mac_dl_dci_payload& dci)
{
  // TODO
}

/// Encodes DCI of UE grant allocation.
inline void encode_dci(const mac_cell_creation_request& cell_cfg, const dl_msg_alloc& msg, mac_dl_dci_payload& dci)
{
  // TODO
}

} // namespace srsgnb

#endif // SRSGNB_DCI_ENCODER_H
