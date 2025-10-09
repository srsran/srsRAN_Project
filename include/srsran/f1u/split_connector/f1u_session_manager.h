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
#include "srsran/ran/s_nssai.h"
#include <map>

namespace srsran {

using f1u_session_list        = std::vector<std::unique_ptr<gtpu_tnl_pdu_session>>;
using f1u_five_qi_session_map = std::map<five_qi_t, f1u_session_list>;
using f1u_s_nssai_session_map = std::map<s_nssai_t, f1u_five_qi_session_map>;

struct f1u_session_maps {
  f1u_session_list        default_gw_sessions;
  f1u_s_nssai_session_map session_maps;
};

class f1u_session_manager
{
public:
  virtual ~f1u_session_manager()                                                           = default;
  virtual gtpu_tnl_pdu_session& get_next_f1u_gateway(s_nssai_t s_nssai, five_qi_t five_qi) = 0;
};

} // namespace srsran
