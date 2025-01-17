/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/f1u/split_connector/f1u_session_manager.h"

namespace srsran {

class f1u_session_manager_impl : public f1u_session_manager
{
public:
  ~f1u_session_manager_impl() override = default;
  explicit f1u_session_manager_impl(const f1u_session_maps& f1u_sessions_);

  gtpu_tnl_pdu_session& get_next_f1u_gateway(five_qi_t five_qi) override;

private:
  srslog::basic_logger&         logger;
  const f1u_session_maps&       f1u_sessions;
  uint32_t                      next_gw = 0;
  std::map<five_qi_t, uint32_t> five_qi_next_gw_map;
};

} // namespace srsran
