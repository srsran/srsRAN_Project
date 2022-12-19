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

/// \brief Modifies the RRC connection of a UE in RRC connected mode. It's used to establishe bearers, etc.
/// TODO Add seqdiag
class rrc_reconfiguration_procedure
{
public:
  rrc_reconfiguration_procedure(rrc_ue_context_t&                         context_,
                                const rrc_reconfiguration_procedure_args& args_,
                                rrc_ue_reconfiguration_proc_notifier&     rrc_ue_notifier_,
                                rrc_ue_event_manager&                     ev_mng_,
                                srslog::basic_logger&                     logger_);

  void operator()(coro_context<async_task<bool>>& ctx);

  static const char* name() { return "RRC Reconfiguration Procedure"; }

private:
  /// \remark Send RRC Reconfiguration, see section 5.3.5 in TS 38.331
  void send_rrc_reconfiguration();

  rrc_ue_context_t&                         context;
  const rrc_reconfiguration_procedure_args& args;

  rrc_ue_reconfiguration_proc_notifier& rrc_ue;    // handler to the parent RRC UE object
  rrc_ue_event_manager&                 event_mng; // event manager for the RRC UE entity
  srslog::basic_logger&                 logger;

  rrc_transaction               transaction;
  eager_async_task<rrc_outcome> task;

  const unsigned timeout_ms = 1000; // arbitrary timeout for RRC Reconfig procedure, UE will be removed if timer fires
  bool           procedure_result = false;
};

/// \brief Fills ASN.1 RRC Reconfiguration struct.
/// \param[out] rrc_reconfig The RRC reconfig ASN.1 struct to fill.
/// \param[in] rrc_transaction_id The RRC transaction id.
/// \param[in] args The fields to filled as optional values.
inline void fill_asn1_rrc_reconfig_msg(asn1::rrc_nr::rrc_recfg_s&                rrc_reconfig,
                                       uint8_t                                   rrc_transaction_id,
                                       const rrc_reconfiguration_procedure_args& args)
{
  using namespace asn1::rrc_nr;
  rrc_recfg_ies_s& reconfig_ies   = rrc_reconfig.crit_exts.set_rrc_recfg();
  rrc_reconfig.rrc_transaction_id = rrc_transaction_id;

  // set redio bearer config
  if (args.radio_bearer_cfg.has_value()) {
    reconfig_ies.radio_bearer_cfg_present = true;
    reconfig_ies.radio_bearer_cfg         = args.radio_bearer_cfg.value();
  }

  // non-critical extensions, i.e. rrc_recfg_v1530_ies_s
  if (args.master_cell_group_config.has_value() || args.nas_pdu.has_value()) {
    reconfig_ies.non_crit_ext_present = true;
  }

  if (args.master_cell_group_config.has_value()) {
    reconfig_ies.non_crit_ext.master_cell_group = args.master_cell_group_config.value();
  }

  if (args.nas_pdu.has_value()) {
    // Only one NAS PDU supported
    reconfig_ies.non_crit_ext.ded_nas_msg_list.resize(1);
    reconfig_ies.non_crit_ext.ded_nas_msg_list[0] = args.nas_pdu.value();
  }
}

} // namespace srs_cu_cp
} // namespace srsgnb
