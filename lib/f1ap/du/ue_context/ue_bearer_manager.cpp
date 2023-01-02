/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ue_bearer_manager.h"
#include "../procedures/f1ap_du_event_manager.h"
#include "f1c_du_bearer_impl.h"
#include "srsgnb/f1u/du/f1u_bearer_factory.h"

using namespace srsgnb;
using namespace srs_du;

void ue_bearer_manager::add_srb0_f1c_bearer(f1c_rx_sdu_notifier&        f1c_rx_sdu_notif,
                                            const asn1::f1ap::nr_cgi_s& pcell_cgi,
                                            const byte_buffer&          du_cu_rrc_container,
                                            f1ap_event_manager&         ev_mng)
{
  f1c_bearers.emplace(0,
                      std::make_unique<f1c_srb0_du_bearer>(
                          ue_ctx, pcell_cgi, du_cu_rrc_container, f1c_notifier, f1c_rx_sdu_notif, ev_mng));
}

void ue_bearer_manager::add_f1c_bearer(srb_id_t srb_id, f1c_rx_sdu_notifier& rx_sdu_notif)
{
  srsgnb_assert(srb_id != srb_id_t::srb0, "This function is only for SRB1, SRB2 or SRB3");

  f1c_bearers.emplace(srb_id_to_uint(srb_id),
                      std::make_unique<f1c_other_srb_du_bearer>(ue_ctx, srb_id, f1c_notifier, rx_sdu_notif));
}
