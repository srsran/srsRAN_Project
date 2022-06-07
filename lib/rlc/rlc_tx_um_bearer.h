/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_RLC_TX_UM_BEARER_H
#define SRSGNB_RLC_TX_UM_BEARER_H

#include "rlc_common_bearer.h"
#include "srsgnb/rlc/rlc.h"
#include "srsgnb/srslog/srslog.h"

namespace srsgnb {

class rlc_tx_um_bearer : public rlc_common_bearer, public rlc_tx_sdu_handler
{
public:
  rlc_tx_um_bearer(du_ue_index_t du_index, lcid_t lcid, rlc_tx_upper_layer_control_plane& upper_cp) :
    rlc_common_bearer(du_index, lcid), upper_cp(upper_cp)
  {}

  void handle_sdu(byte_buffer sdu) override
  {
    // TODO
  }

private:
  rlc_tx_upper_layer_control_plane& upper_cp;
};

} // namespace srsgnb

#endif // SRSGNB_RLC_IMPL_H
