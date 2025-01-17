/*
 *
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/f1u/split_connector/f1u_session_manager_factory.h"
#include "f1u_session_manager_impl.h"

using namespace srsran;

std::unique_ptr<f1u_session_manager> srsran::create_f1u_cu_up_session_manager(const f1u_session_maps& f1u_sessions)
{
  return std::make_unique<f1u_session_manager_impl>(f1u_sessions);
}
