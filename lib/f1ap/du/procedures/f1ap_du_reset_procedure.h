/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/asn1/f1ap/f1ap.h"
#include "srsran/asn1/f1ap/f1ap_pdu_contents.h"
#include "srsran/f1ap/du/f1ap_du.h"
#include "srsran/support/async/async_task.h"

namespace srsran {

class f1ap_message_notifier;

namespace srs_du {

class f1ap_du_ue_manager;

/// Implementation of the F1AP RESET procedure as per TS 38.473, section 8.2.1.
class reset_procedure
{
public:
  reset_procedure(const asn1::f1ap::reset_s& msg,
                  f1ap_du_configurator&      du_mng,
                  f1ap_du_ue_manager&        ue_mng,
                  f1ap_message_notifier&     msg_notifier);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  const char* name() const { return "Reset"; }

  async_task<void>           handle_reset();
  std::vector<du_ue_index_t> create_ues_to_reset() const;
  void                       send_ack() const;

  const asn1::f1ap::reset_s msg;
  f1ap_du_configurator&     du_mng;
  f1ap_du_ue_manager&       ue_mng;
  f1ap_message_notifier&    msg_notifier;
  srslog::basic_logger&     logger;

  std::vector<asn1::f1ap::ue_associated_lc_f1_conn_item_s> ues_reset;
};

} // namespace srs_du
} // namespace srsran
