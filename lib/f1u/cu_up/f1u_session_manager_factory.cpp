/*
 *
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/f1u/cu_up/f1u_session_manager_factory.h"
#include "f1u_session_manager_impl.h"
#include "srsran/f1u/cu_up/f1u_gateway.h"

using namespace srsran;
using namespace srs_cu_up;

std::unique_ptr<f1u_session_manager>
srsran::srs_cu_up::create_f1u_cu_up_session_manager(const std::vector<std::unique_ptr<gtpu_tnl_pdu_session>>& f1u_gws)
{
  return std::make_unique<f1u_session_manager_impl>(f1u_gws);
}
