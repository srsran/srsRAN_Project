/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "f1ap_ue_context.h"
#include "srsgnb/adt/slotted_array.h"
#include "srsgnb/f1ap/common/f1ap_common.h"
#include "srsgnb/f1ap/du/f1c_bearer.h"
#include "srsgnb/f1ap/du/f1c_rx_sdu_notifier.h"
#include "srsgnb/f1u/du/f1u_bearer.h"
#include "srsgnb/f1u/du/f1u_rx_sdu_notifier.h"
#include "srsgnb/f1u/du/f1u_tx_pdu_notifier.h"
#include "srsgnb/ran/lcid.h"

namespace srsgnb {
namespace srs_du {

class f1ap_event_manager;

class ue_bearer_manager
{
public:
  ue_bearer_manager(f1ap_ue_context& ue_ctx_, f1c_message_notifier& f1c_notif_) :
    ue_ctx(ue_ctx_), f1c_notifier(f1c_notif_)
  {
  }

  void add_srb0_f1c_bearer(f1c_rx_sdu_notifier&        f1c_rx_sdu_notif,
                           const asn1::f1ap::nr_cgi_s& pcell_cgi,
                           const byte_buffer&          du_cu_rrc_container,
                           f1ap_event_manager&         ev_mng);

  void add_f1c_bearer(srb_id_t srb_id, f1c_rx_sdu_notifier& rx_sdu_notif);

  f1c_bearer* find_srb(srb_id_t srb_id)
  {
    return f1c_bearers.contains(srb_id_to_uint(srb_id)) ? f1c_bearers[srb_id_to_uint(srb_id)].get() : nullptr;
  }
  const f1c_bearer* find_srb(srb_id_t srb_id) const
  {
    return f1c_bearers.contains(srb_id_to_uint(srb_id)) ? f1c_bearers[srb_id_to_uint(srb_id)].get() : nullptr;
  }

private:
  f1ap_ue_context&      ue_ctx;
  f1c_message_notifier& f1c_notifier;

  slotted_array<std::unique_ptr<f1c_bearer>, MAX_NOF_SRBS> f1c_bearers;
};

} // namespace srs_du
} // namespace srsgnb
