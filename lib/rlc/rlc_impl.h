/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_RLC_IMPL_H
#define SRSGNB_RLC_IMPL_H

#include "rlc_common_bearer.h"
#include "srsgnb/rlc/rlc.h"
#include "srsgnb/srslog/srslog.h"

namespace srsgnb {

class rlc_ul_um_bearer : public rlc_common_bearer, public rlc_pdu_handler
{
public:
  rlc_ul_um_bearer(du_ue_index_t du_index, lcid_t lcid, rlc_sdu_rx_notifier& notifier) :
    rlc_common_bearer(du_index, lcid), notifier(notifier)
  {}

  void handle_pdu(byte_buffer pdu) override { notifier.on_new_sdu(du_index, lcid, std::move(pdu)); }

private:
  rlc_sdu_rx_notifier& notifier;
};

} // namespace srsgnb

#endif // SRSGNB_RLC_IMPL_H
