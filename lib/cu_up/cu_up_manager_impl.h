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

#include "adapters/gtpu_adapters.h"
#include "ue_manager.h"
#include "srsran/cu_up/cu_up_configuration.h"
#include "srsran/cu_up/cu_up_manager.h"
#include "srsran/e1ap/cu_up/e1ap_cu_up.h"
#include "srsran/gtpu/gtpu_teid_pool.h"
#include <memory>

namespace srsran::srs_cu_up {

class cu_up_manager_impl final : public cu_up_manager
{
public:
  explicit cu_up_manager_impl(const cu_up_configuration&    cfg_,
                              e1ap_interface&               e1ap,
                              gtpu_network_gateway_adapter& gtpu_gw_adapter,
                              gtpu_demux&                   ngu_demux,
                              gtpu_teid_pool&               n3_teid_allocator,
                              gtpu_teid_pool&               f1u_teid_allocator);
  ~cu_up_manager_impl() override = default;

  e1ap_bearer_context_setup_response
  handle_bearer_context_setup_request(const e1ap_bearer_context_setup_request& msg) override;

  async_task<e1ap_bearer_context_modification_response>
  handle_bearer_context_modification_request(const e1ap_bearer_context_modification_request& msg) override;

  void handle_bearer_context_release_command(const e1ap_bearer_context_release_command& msg) override;

  void schedule_ue_async_task(srs_cu_up::ue_index_t ue_index, async_task<void> task) override;

  // cu_up_e1ap_connection_notifier
  void on_e1ap_connection_establish() override { e1ap_connected = true; }
  void on_e1ap_connection_drop() override { e1ap_connected = false; }
  bool e1ap_is_connected() override { return e1ap_connected; }

  size_t get_nof_ues() override { return ue_mng->get_nof_ues(); }

private:
  void on_statistics_report_timer_expired();

  e1ap_bearer_context_modification_response
  handle_bearer_context_modification_request_impl(ue_context&                                     ue_ctxt,
                                                  const e1ap_bearer_context_modification_request& msg);

  cu_up_configuration cfg;

  // logger
  srslog::basic_logger& logger = srslog::fetch_basic_logger("CU-UP", false);

  // Components
  std::atomic<bool>           e1ap_connected = {false};
  std::unique_ptr<ue_manager> ue_mng;

  unique_timer statistics_report_timer;
};

} // namespace srsran::srs_cu_up
