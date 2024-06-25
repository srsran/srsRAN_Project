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

#include "srsran/cu_up/cu_up.h"
#include "srsran/e1ap/cu_up/e1ap_cu_up_bearer_context_update.h"
#include "srsran/gtpu/ngu_gateway.h"

namespace srsran {

/// \brief CU-UP wrapper. Wraps a CU-UP interface and a NGU gateway.
///
/// The wrapper purpose is to manage the life of the given NGU gateway and the CU-UP interface.
class cu_up_wrapper : public srs_cu_up::cu_up_interface
{
public:
  cu_up_wrapper(std::unique_ptr<srs_cu_up::ngu_gateway> gateway_, std::unique_ptr<srs_cu_up::cu_up_interface> cu_up_);

  // See interface for documentation.
  void start() override;

  // See interface for documentation.
  void stop() override;

  // See interface for documentation.
  std::optional<uint16_t> get_n3_bind_port() override;

  // See interface for documentation.
  e1ap_message_handler& get_e1ap_message_handler() override;

  // See interface for documentation.
  void handle_bearer_context_release_command(const srs_cu_up::e1ap_bearer_context_release_command& msg) override;

  // See interface for documentation.
  bool e1ap_is_connected() override;

  // See interface for documentation.
  void on_e1ap_connection_establish() override;

  // See interface for documentation.
  void on_e1ap_connection_drop() override;

  // See interface for documentation.
  srs_cu_up::e1ap_bearer_context_setup_response
  handle_bearer_context_setup_request(const srs_cu_up::e1ap_bearer_context_setup_request& msg) override;

  // See interface for documentation.
  async_task<srs_cu_up::e1ap_bearer_context_modification_response>
  handle_bearer_context_modification_request(const srs_cu_up::e1ap_bearer_context_modification_request& msg) override;

  /// \brief Schedule an async task for an UE.
  /// Can be used to initiate UE routines.
  void schedule_ue_async_task(srs_cu_up::ue_index_t ue_index, async_task<void> task) override;

private:
  std::unique_ptr<srs_cu_up::ngu_gateway>     gateway;
  std::unique_ptr<srs_cu_up::cu_up_interface> cu_up;
};

} // namespace srsran
