/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_RLC_RX_UM_BEARER_H
#define SRSGNB_RLC_RX_UM_BEARER_H

#include "rlc_rx_entity.h"
#include "rlc_sdu_queue.h"
#include "rlc_um_pdu.h"

namespace srsgnb {

class rlc_rx_um_entity : public rlc_rx_entity
{
private:
  // Config storrage
  const rlc_um_config cfg;

public:
  rlc_rx_um_entity(du_ue_index_t                     du_index,
                   lcid_t                            lcid,
                   const rlc_um_config&              config,
                   rlc_rx_upper_layer_data_notifier& upper_dn) :
    rlc_rx_entity(du_index, lcid, upper_dn), cfg(config)
  {
    (void)cfg; // TODO unused
  }

  void handle_pdu(shared_byte_buffer_view buf) override { upper_dn.on_new_sdu(std::move(buf)); }
};

} // namespace srsgnb

#endif
