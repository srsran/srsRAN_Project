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

#include "srsran/adt/concurrent_queue.h"
#include "srsran/cu_cp/cu_up_repository.h"
#include "srsran/e1ap/common/e1ap_message.h"

namespace srsran {
namespace srs_cu_cp {

/// \brief Mock class for the interface between CU-UP and CU-CP that accounts for the fact that the CU-CP may push
/// PDUs from different threads.
class synchronized_mock_cu_up
{
public:
  synchronized_mock_cu_up(cu_up_repository& cu_cp_e1_handler_) : cu_cp_node(&cu_cp_e1_handler_) {}

  void handle_rx_pdu(const e1ap_message& rx_pdu);

  bool request_new_cu_up_connection();

  bool try_pop_rx_pdu(e1ap_message& pdu);

private:
  using rx_pdu_queue = concurrent_queue<e1ap_message,
                                        concurrent_queue_policy::locking_mpmc,
                                        concurrent_queue_wait_policy::condition_variable>;

  cu_up_repository* cu_cp_node = nullptr;

  rx_pdu_queue rx_pdus{1024};

  // Notifier used by the CU-UP to send PDUs to the CU-CP.
  std::unique_ptr<e1ap_message_notifier> tx_pdu_notifier;
};

} // namespace srs_cu_cp
} // namespace srsran