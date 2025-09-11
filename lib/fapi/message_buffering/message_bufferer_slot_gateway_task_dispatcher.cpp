/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "message_bufferer_slot_gateway_task_dispatcher.h"
#include "srsran/fapi/messages/dl_tti_request.h"
#include "srsran/fapi/messages/tx_data_request.h"
#include "srsran/fapi/messages/ul_dci_request.h"
#include "srsran/fapi/messages/ul_tti_request.h"
#include "srsran/srslog/srslog.h"
#include "srsran/support/executors/task_executor.h"
#include "srsran/support/rtsan.h"

using namespace srsran;
using namespace fapi;

message_bufferer_slot_gateway_task_dispatcher::message_bufferer_slot_gateway_task_dispatcher(
    unsigned                    sector_id_,
    unsigned                    l2_nof_slots_ahead,
    subcarrier_spacing          scs_,
    fapi::slot_message_gateway& gateway,
    task_executor&              executor_) :
  sector_id(sector_id_),
  scs(scs_),
  logger(srslog::fetch_basic_logger("FAPI")),
  executor(executor_),
  message_bufferer_gateway(sector_id, l2_nof_slots_ahead, scs_, gateway)
{
}

void message_bufferer_slot_gateway_task_dispatcher::dl_tti_request(const dl_tti_request_message& msg)
{
  if (!executor.defer(
          [this, msg]() noexcept SRSRAN_RTSAN_NONBLOCKING { message_bufferer_gateway.handle_dl_tti_request(msg); })) {
    logger.warning("Sector#{}: Failed to cache DL_TTI.request message for slot '{}'",
                   sector_id,
                   slot_point(scs, msg.sfn, msg.slot));
  }
}

void message_bufferer_slot_gateway_task_dispatcher::ul_tti_request(const ul_tti_request_message& msg)
{
  if (!executor.defer(
          [this, msg]() noexcept SRSRAN_RTSAN_NONBLOCKING { message_bufferer_gateway.handle_ul_tti_request(msg); })) {
    logger.warning("Sector#{}: Failed to cache UL_TTI.request message for slot '{}'",
                   sector_id,
                   slot_point(scs, msg.sfn, msg.slot));
  }
}

void message_bufferer_slot_gateway_task_dispatcher::ul_dci_request(const ul_dci_request_message& msg)
{
  if (!executor.defer(
          [this, msg]() noexcept SRSRAN_RTSAN_NONBLOCKING { message_bufferer_gateway.handle_ul_dci_request(msg); })) {
    logger.warning("Sector#{}: Failed to cache UL_DCI.request message for slot '{}'",
                   sector_id,
                   slot_point(scs, msg.sfn, msg.slot));
  }
}

void message_bufferer_slot_gateway_task_dispatcher::tx_data_request(const tx_data_request_message& msg)
{
  if (!executor.defer(
          [this, msg]() noexcept SRSRAN_RTSAN_NONBLOCKING { message_bufferer_gateway.handle_tx_data_request(msg); })) {
    logger.warning("Sector#{}: Failed to cache TX_Data.request message for slot '{}'",
                   sector_id,
                   slot_point(scs, msg.sfn, msg.slot));
  }
}

void message_bufferer_slot_gateway_task_dispatcher::update_current_slot(slot_point slot)
{
  message_bufferer_gateway.update_current_slot(slot);
}

void message_bufferer_slot_gateway_task_dispatcher::forward_cached_messages(slot_point slot)
{
  if (!executor.defer([this, slot]() noexcept SRSRAN_RTSAN_NONBLOCKING {
        message_bufferer_gateway.forward_cached_messages(slot);
      })) {
    logger.warning("Sector#{}: Failed to dispatch cached messages for slot '{}'", sector_id, slot);
  }
}
