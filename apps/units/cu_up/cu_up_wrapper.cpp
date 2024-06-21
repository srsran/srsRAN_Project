/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_up_wrapper.h"

using namespace srsran;

cu_up_wrapper::cu_up_wrapper(std::unique_ptr<srs_cu_up::ngu_gateway>     gateway_,
                             std::unique_ptr<srs_cu_up::cu_up_interface> cu_up_) :
  gateway(std::move(gateway_)), cu_up(std::move(cu_up_))
{
  srsran_assert(gateway, "Invlid NGU gateway");
  srsran_assert(cu_up, "Invalid CU-UP");
}

void cu_up_wrapper::start()
{
  cu_up->start();
}

void cu_up_wrapper::stop()
{
  cu_up->stop();
}

std::optional<uint16_t> cu_up_wrapper::get_n3_bind_port()
{
  return cu_up->get_n3_bind_port();
}

e1ap_message_handler& cu_up_wrapper::get_e1ap_message_handler()
{
  return cu_up->get_e1ap_message_handler();
}

void cu_up_wrapper::handle_bearer_context_release_command(const srs_cu_up::e1ap_bearer_context_release_command& msg)
{
  cu_up->handle_bearer_context_release_command(msg);
}

bool cu_up_wrapper::e1ap_is_connected()
{
  return cu_up->e1ap_is_connected();
}

void cu_up_wrapper::on_e1ap_connection_establish()
{
  cu_up->on_e1ap_connection_establish();
}

void cu_up_wrapper::on_e1ap_connection_drop()
{
  cu_up->on_e1ap_connection_drop();
}

srs_cu_up::e1ap_bearer_context_setup_response
cu_up_wrapper::handle_bearer_context_setup_request(const srs_cu_up::e1ap_bearer_context_setup_request& msg)
{
  return cu_up->handle_bearer_context_setup_request(msg);
}

// TODO remove from public interface
async_task<srs_cu_up::e1ap_bearer_context_modification_response>
cu_up_wrapper::handle_bearer_context_modification_request(
    const srs_cu_up::e1ap_bearer_context_modification_request& msg)
{
  return cu_up->handle_bearer_context_modification_request(msg);
}

// TODO remove from public interface
void cu_up_wrapper::schedule_ue_async_task(srs_cu_up::ue_index_t ue_index, async_task<void> task) {}
