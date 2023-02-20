/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "f1u_bearer_logger.h"
#include "srsran/f1u/cu_up/f1u_bearer.h"
#include "srsran/f1u/cu_up/f1u_rx_delivery_notifier.h"
#include "srsran/f1u/cu_up/f1u_rx_sdu_notifier.h"
#include "srsran/f1u/cu_up/f1u_tx_pdu_notifier.h"
#include "srsran/ran/lcid.h"

namespace srsran {
namespace srs_cu_up {

class f1u_bearer_impl final : public f1u_bearer, public f1u_rx_pdu_handler, public f1u_tx_sdu_handler
{
public:
  f1u_bearer_impl(uint32_t                  ue_index,
                  drb_id_t                  drb_id_,
                  f1u_tx_pdu_notifier&      tx_pdu_notifier_,
                  f1u_rx_delivery_notifier& rx_delivery_notifier_,
                  f1u_rx_sdu_notifier&      rx_sdu_notifier_,
                  f1u_bearer_disconnector&  diconnector_,
                  uint32_t                  ul_teid_);
  f1u_bearer_impl(const f1u_bearer_impl&)            = delete;
  f1u_bearer_impl& operator=(const f1u_bearer_impl&) = delete;

  virtual ~f1u_bearer_impl() { disconnector.disconnect_cu_bearer(ul_teid); }

  virtual f1u_rx_pdu_handler& get_rx_pdu_handler() override { return *this; }
  virtual f1u_tx_sdu_handler& get_tx_sdu_handler() override { return *this; }

  void handle_pdu(nru_ul_message msg) override;
  void handle_sdu(pdcp_tx_pdu sdu) override;
  void discard_sdu(uint32_t pdcp_sn) override;

  uint32_t get_ul_teid() { return ul_teid; }

private:
  f1u_bearer_logger         logger;
  f1u_tx_pdu_notifier&      tx_pdu_notifier;
  f1u_rx_delivery_notifier& rx_delivery_notifier;
  f1u_rx_sdu_notifier&      rx_sdu_notifier;
  f1u_bearer_disconnector&  disconnector;
  uint32_t                  ul_teid;
};

} // namespace srs_cu_up
} // namespace srsran
