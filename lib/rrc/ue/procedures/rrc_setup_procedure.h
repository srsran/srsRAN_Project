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

#include "../rrc_ue_context.h"
#include "rrc_ue_event_manager.h"
#include "srsgnb/asn1/rrc_nr/rrc_nr.h"
#include "srsgnb/rrc/rrc_entity_interface.h"
#include "srsgnb/rrc/rrc_ue_entity_interface.h"
#include "srsgnb/support/async/async_task.h"
#include "srsgnb/support/async/eager_async_task.h"

namespace srsgnb {
namespace srs_cu_cp {

/// Interface between RRC Setup procedure and RRC UE entity.
class rrc_ue_setup_proc_notifier
{
public:
  rrc_ue_setup_proc_notifier()                                                = default;
  virtual ~rrc_ue_setup_proc_notifier()                                       = default;
  virtual void on_new_dl_ccch(const asn1::rrc_nr::dl_ccch_msg_s& dl_ccch_msg) = 0;

  /// Informs the DU processor that the UE shall be released
  virtual void on_ue_delete_request() = 0;
};

class rrc_setup_procedure
{
public:
  rrc_setup_procedure(rrc_ue_context_t&                        context_,
                      const asn1::rrc_nr::rrc_setup_request_s& request_,
                      const byte_buffer&                       du_to_cu_container_,
                      rrc_ue_setup_proc_notifier&              rrc_ue_notifier_,
                      rrc_ue_du_processor_notifier&            du_processor_notifier_,
                      rrc_ue_event_manager&                    ev_mng_,
                      srslog::basic_logger&                    logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "RRC Setup Procedure"; }

private:
  /// Instruct DU processor to create SRB1 bearer.
  void create_srb1();

  /// \remark Send RRC Setup, see section 5.3.3 in TS 36.331
  void send_rrc_setup();

  rrc_ue_context_t                        context;
  const asn1::rrc_nr::rrc_setup_request_s request;
  const byte_buffer&                      du_to_cu_container;
  const asn1::rrc_nr::pdcp_cfg_s          srb1_pdcp_cfg;

  rrc_ue_setup_proc_notifier&   rrc_ue;                // handler to the parent RRC UE object
  rrc_ue_du_processor_notifier& du_processor_notifier; // notifier to the DU processor
  rrc_ue_event_manager&         event_mng;             // event manager for the RRC UE entity
  srslog::basic_logger&         logger;

  rrc_transaction               transaction;
  eager_async_task<rrc_outcome> task;

  const unsigned rrc_setup_timeout_ms =
      1000; // arbitrary timeout for RRC Setup procedure, UE will be removed if timer fires
};

} // namespace srs_cu_cp
} // namespace srsgnb
