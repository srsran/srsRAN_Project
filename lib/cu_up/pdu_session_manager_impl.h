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
#include "srsgnb/cu_up/cu_up_types.h"
#include "srsgnb/f1u/cu_up/f1u_gateway.h"
#include "srsgnb/gtpu/gtpu_demux.h"
#include "srsgnb/support/timers.h"
#include <map>

namespace srsgnb {

namespace srs_cu_up {

class pdu_session_manager_impl : public pdu_session_manager_ctrl
{
public:
  pdu_session_manager_impl(ue_index_t            ue_index_,
                           srslog::basic_logger& logger_,
                           timer_manager&        timers_,
                           f1u_cu_up_gateway&    f1u_gw_,
                           gtpu_demux_ctrl&      ngu_demux_);
  ~pdu_session_manager_impl() = default;

  pdu_session_setup_result setup_pdu_session(const asn1::e1ap::pdu_session_res_to_setup_item_s& session) override;
  void                     remove_pdu_session(uint8_t pdu_session_id) override;
  size_t                   get_nof_pdu_sessions() override;

private:
  uint32_t allocate_local_teid(uint8_t pdu_session_id);

  ue_index_t                                      ue_index;
  srslog::basic_logger&                           logger;
  timer_manager&                                  timers;
  gtpu_demux_ctrl&                                ngu_demux;
  f1u_cu_up_gateway&                              f1u_gw;
  std::map<uint8_t, std::unique_ptr<pdu_session>> pdu_sessions; // key is pdu_session_id
};

} // namespace srs_cu_up

} // namespace srsgnb
