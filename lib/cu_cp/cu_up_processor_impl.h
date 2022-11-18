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

#include "adapters/e1_adapters.h"
#include "adapters/ngc_adapters.h"
#include "srsgnb/adt/slot_array.h"
#include "srsgnb/cu_cp/cu_cp_types.h"
#include "srsgnb/cu_cp/cu_up_processor_config.h"
#include "srsgnb/e1_interface/cu_cp/e1_cu_cp.h"
#include "srsgnb/support/async/async_task_loop.h"
#include <string>

namespace srsgnb {
namespace srs_cu_cp {

class cu_up_processor_impl : public cu_up_processor_interface
{
public:
  cu_up_processor_impl(const cu_up_processor_config_t cu_up_processor_config_, e1_message_notifier& e1_notifier_);
  ~cu_up_processor_impl() = default;

  // message handlers
  void handle_cu_up_e1_setup_request(const srsgnb::cu_up_e1_setup_request_message& msg) override;

  // getter functions

  e1_message_handler& get_e1_message_handler() override { return *e1; };

private:
  // E1 senders

  /// \brief Create and transmit the GNB-CU-UP E1 Setup response message.
  /// \param[in] du_ctxt The context of the DU that should receive the message.
  void send_cu_up_e1_setup_response();

  /// \brief Create and transmit the GNB-CU-UP E1 Setup failure message.
  /// \param[in] cause The cause of the failure.
  void send_cu_up_e1_setup_failure(asn1::e1ap::cause_c::types::options cause);

  srslog::basic_logger&    logger = srslog::fetch_basic_logger("CU-CP");
  cu_up_processor_config_t cfg;

  e1_message_notifier& e1_notifier;

  // timers associated with a given DU.
  timer_manager timer_db;

  // Components
  std::unique_ptr<e1_interface>  e1;
  std::unique_ptr<ngc_interface> ngc;

  // E1 to CU-UP processor adapter
  e1_cu_up_processor_adapter e1_ev_notifier;
};

} // namespace srs_cu_cp
} // namespace srsgnb
