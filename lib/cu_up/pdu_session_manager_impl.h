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

#include "pdu_session_manager.h"
#include "srsgnb/support/timers.h"
#include <map>

namespace srsgnb {

namespace srs_cu_up {

class pdu_session_manager_impl : public pdu_session_manager_ctrl
{
public:
  pdu_session_manager_impl(srslog::basic_logger& logger_, timer_manager& timers_);
  ~pdu_session_manager_impl() = default;

  pdu_session_setup_result setup_pdu_session(const asn1::e1ap::pdu_session_res_to_setup_item_s& session) override;
  void                     remove_pdu_session(uint16_t pdu_session_id) override;
  size_t                   get_nof_pdu_sessions() override;

private:
  srslog::basic_logger&                            logger;
  timer_manager&                                   timers;
  std::map<uint16_t, std::unique_ptr<pdu_session>> pdu_sessions; // key is pdu_session_id
};

} // namespace srs_cu_up

} // namespace srsgnb
