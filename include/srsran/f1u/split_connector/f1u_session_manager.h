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

#include "srsran/gtpu/gtpu_gateway.h"
#include "srsran/ran/qos/five_qi.h"
#include <map>

namespace srsran {

struct f1u_session_maps {
  std::vector<std::unique_ptr<gtpu_tnl_pdu_session>>                      default_gw_sessions;
  std::map<five_qi_t, std::vector<std::unique_ptr<gtpu_tnl_pdu_session>>> five_qi_gw_sessions;
};

class f1u_session_manager
{
public:
  virtual ~f1u_session_manager()                                        = default;
  virtual gtpu_tnl_pdu_session& get_next_f1u_gateway(five_qi_t five_qi) = 0;
};

} // namespace srsran
