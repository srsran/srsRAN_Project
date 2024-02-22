/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "buffered_slot_gateway_impl.h"

using namespace srsran;
using namespace fapi;

buffered_slot_gateway_impl::buffered_slot_gateway_impl(unsigned int          l2_nof_slots_ahead_,
                                                       subcarrier_spacing    scs_,
                                                       slot_message_gateway& gateway_) :
  l2_nof_slots_ahead(l2_nof_slots_ahead_), scs(scs_), gateway(gateway_), logger(srslog::fetch_basic_logger("FAPI"))
{
  // Pool size is 1 unit bigger than the number of slots the L2 is ahead of the L1, in case that an incoming message
  // arrives before the notification of a new slot.
  unsigned pool_size = l2_nof_slots_ahead + 1;

  srsran_assert(pool_size <= MAX_NUM_BUFFERED_MESSAGES,
                "The requested pool size '{}' is bigger than the supported '{}'",
                pool_size,
                MAX_NUM_BUFFERED_MESSAGES);

  dl_tti_pool.resize(pool_size);
  ul_tti_pool.resize(pool_size);
  ul_dci_pool.resize(pool_size);
  tx_data_pool.resize(pool_size);
}

template <typename T, typename Function>
void buffered_slot_gateway_impl::handle_message(const T& msg, span<optional<T>> pool, Function func)
{
  slot_point msg_slot(scs, msg.sfn, msg.slot);
  slot_point current_slot = get_current_slot();

  int difference = msg_slot - current_slot;

  if (difference > int(l2_nof_slots_ahead)) {
    logger.warning("Skipping FAPI message as the difference between the slot of the message and the current slot '{}' "
                   "is bigger than the configured delay '{}'",
                   difference,
                   l2_nof_slots_ahead);

    return;
  }

  // Same slot or older, do not need to store.
  if (difference <= 0) {
    func(msg);

    return;
  }

  optional<T>& msg_pool_entry = pool[msg_slot.system_slot() % pool.size()];

  if (msg_pool_entry) {
    logger.warning("Detected unsent cached FAPI message type '{}' for slot '{}'",
                   msg_pool_entry->message_type,
                   slot_point(scs, msg_pool_entry->sfn, msg_pool_entry->slot));
  }

  msg_pool_entry.emplace(msg);
}

/// Sends the message cached in the given pool for the given slot using the given function and clears the pool
/// entry.
template <typename T, typename Function>
static void send_message(slot_point            slot,
                         subcarrier_spacing    scs,
                         srslog::basic_logger& logger,
                         span<optional<T>>     pool,
                         Function              func)
{
  optional<T>& pool_entry = pool[slot.system_slot() % pool.size()];
  if (!pool_entry) {
    return;
  }

  slot_point pool_message_slot(scs, pool_entry->sfn, pool_entry->slot);
  if (pool_message_slot == slot) {
    func(*pool_entry);
  } else {
    logger.warning(
        "Detected unsent cached FAPI message id '{}' for slot '{}'", pool_entry->message_type, pool_message_slot);
  }

  pool_entry.reset();
}

void buffered_slot_gateway_impl::forward_cached_messages(slot_point slot)
{
  send_message<dl_tti_request_message>(
      slot, scs, logger, dl_tti_pool, [this](const dl_tti_request_message& msg) { gateway.dl_tti_request(msg); });

  send_message<ul_tti_request_message>(
      slot, scs, logger, ul_tti_pool, [this](const ul_tti_request_message& msg) { gateway.ul_tti_request(msg); });

  send_message<ul_dci_request_message>(
      slot, scs, logger, ul_dci_pool, [this](const ul_dci_request_message& msg) { gateway.ul_dci_request(msg); });

  send_message<tx_data_request_message>(
      slot, scs, logger, tx_data_pool, [this](const tx_data_request_message& msg) { gateway.tx_data_request(msg); });
}

void buffered_slot_gateway_impl::handle_dl_tti_request(const dl_tti_request_message& msg)
{
  handle_message(msg,
                 span<optional<dl_tti_request_message>>(dl_tti_pool),
                 [this](const dl_tti_request_message& message) { gateway.dl_tti_request(message); });
}

void buffered_slot_gateway_impl::handle_ul_tti_request(const ul_tti_request_message& msg)
{
  handle_message(msg,
                 span<optional<ul_tti_request_message>>(ul_tti_pool),
                 [this](const ul_tti_request_message& message) { gateway.ul_tti_request(message); });
}

void buffered_slot_gateway_impl::handle_ul_dci_request(const ul_dci_request_message& msg)
{
  handle_message(msg,
                 span<optional<ul_dci_request_message>>(ul_dci_pool),
                 [this](const ul_dci_request_message& message) { gateway.ul_dci_request(message); });
}

void buffered_slot_gateway_impl::handle_tx_data_request(const tx_data_request_message& msg)
{
  handle_message(msg,
                 span<optional<tx_data_request_message>>(tx_data_pool),
                 [this](const tx_data_request_message& message) { gateway.tx_data_request(message); });
}
