/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/pdcp/pdcp_config.h"
#include "srsran/pdcp/pdcp_entity.h"
#include "srsran/pdcp/pdcp_rx.h"
#include "srsran/pdcp/pdcp_tx.h"
#include "srsran/ran/rb_id.h"
#include "srsran/support/timers.h"
#include <memory>

/// This factory header file depends on the PDCP entity interfaces (see above include list). It is kept separate as
/// clients of the PDCP entity interfaces dont need to call factory methods.

namespace srsran {

struct pdcp_entity_creation_message {
  uint32_t                        ue_index;
  rb_id_t                         rb_id;
  pdcp_config                     config;
  pdcp_tx_lower_notifier*         tx_lower;
  pdcp_tx_upper_control_notifier* tx_upper_cn;
  pdcp_rx_upper_data_notifier*    rx_upper_dn;
  pdcp_rx_upper_control_notifier* rx_upper_cn;
  timer_factory                   ue_dl_timer_factory;
  timer_factory                   ue_ul_timer_factory;
  timer_factory                   ue_ctrl_timer_factory;
  task_executor*                  ue_dl_executor;
  task_executor*                  ue_ul_executor;
  task_executor*                  crypto_executor;
  uint32_t                        max_nof_crypto_workers;
};

/// Creates an instance of a PDCP entity.
std::unique_ptr<pdcp_entity> create_pdcp_entity(pdcp_entity_creation_message& msg);

} // namespace srsran
