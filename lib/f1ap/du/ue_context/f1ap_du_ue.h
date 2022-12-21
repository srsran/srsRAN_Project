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
#include "ue_bearer_manager.h"
#include "srsgnb/adt/slotted_array.h"
#include "srsgnb/adt/stable_id_map.h"
#include "srsgnb/f1ap/common/f1c_types.h"
#include "srsgnb/f1ap/du/f1ap_du.h"
#include "srsgnb/ran/du_types.h"
#include <mutex>
#include <unordered_map>

namespace srsgnb {
namespace srs_du {

class f1ap_du_ue
{
public:
  f1ap_du_ue(du_ue_index_t         ue_index_,
             gnb_du_ue_f1ap_id_t   gnb_f1_du_ue_id_,
             f1c_du_configurator&  du_handler_,
             f1c_message_notifier& f1c_msg_notifier_) :
    context(ue_index_, gnb_f1_du_ue_id_),
    f1c_msg_notifier(f1c_msg_notifier_),
    du_handler(du_handler_),
    bearers(context, f1c_msg_notifier)
  {
  }

  f1ap_ue_context       context;
  f1c_message_notifier& f1c_msg_notifier;
  f1c_du_configurator&  du_handler;
  ue_bearer_manager     bearers;

  /// \brief Handles UE CONTEXT SETUP REQUEST as per TS38.473, Section 8.3.1.
  void handle_ue_context_setup_request(const asn1::f1ap::ue_context_setup_request_s& msg);

  /// \brief Handles UE CONTEXT MODIFICATION REQUEST as per TS38.473, Section 8.3.2.
  void handle_ue_context_modification_request(const asn1::f1ap::ue_context_mod_request_s& msg);
};

} // namespace srs_du
} // namespace srsgnb
