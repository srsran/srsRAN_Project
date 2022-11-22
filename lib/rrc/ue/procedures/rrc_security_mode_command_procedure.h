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
#include "srsgnb/rrc/rrc_du.h"
#include "srsgnb/rrc/rrc_ue.h"
#include "srsgnb/support/async/async_task.h"
#include "srsgnb/support/async/eager_async_task.h"

namespace srsgnb {
namespace srs_cu_cp {

struct rrc_security_context {
  security::sec_as_key k;
  std::array<bool, 3>  supported_int_algos;
  std::array<bool, 3>  supported_enc_algos;
};

/// \brief Handles the setup of AS security keys in the RRC UE.
/// TODO Add seqdiag
class rrc_security_mode_command_procedure
{
public:
  rrc_security_mode_command_procedure(rrc_ue_context_t&                           context_,
                                      rrc_security_context                        sec_ctx,
                                      const byte_buffer&                          du_to_cu_container_,
                                      rrc_ue_security_mode_command_proc_notifier& rrc_ue_notifier_,
                                      rrc_ue_event_manager&                       ev_mng_,
                                      srslog::basic_logger&                       logger_);

  void operator()(coro_context<async_task<void>>& ctx);

  static const char* name() { return "RRC Setup Procedure"; }

private:
  /// \remark Select security algorithms based on the UE capabilities
  /// provided by the NGAP.
  bool select_security_algo();

  /// \remark Send RRC Security Mode Command, see section 5.3.3 in TS 36.331
  void send_rrc_security_mode_command();

  rrc_ue_context_t                        context;
  rrc_security_context                    sec_ctx;
  const asn1::rrc_nr::rrc_setup_request_s request;
  const byte_buffer&                      du_to_cu_container;
  const asn1::rrc_nr::pdcp_cfg_s          srb1_pdcp_cfg;

  rrc_ue_security_mode_command_proc_notifier& rrc_ue;    // handler to the parent RRC UE object
  rrc_ue_event_manager&                       event_mng; // event manager for the RRC UE entity
  srslog::basic_logger&                       logger;

  rrc_transaction               transaction;
  eager_async_task<rrc_outcome> task;

  security::integrity_algorithm int_algo  = {};
  security::ciphering_algorithm ciph_algo = {};

  const unsigned rrc_smc_timeout_ms =
      1000; // arbitrary timeout for RRC SMC procedure, UE will be removed if timer fires
};

} // namespace srs_cu_cp
} // namespace srsgnb
