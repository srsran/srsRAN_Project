/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "amf_connection_removal_routine.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/support/async/coroutine.h"

using namespace srsran;
using namespace srs_cu_cp;

#ifndef SRSRAN_HAS_ENTERPRISE

async_task<void>
srsran::srs_cu_cp::start_amf_connection_removal(ngap_repository&                                    ngap_db,
                                                std::unordered_map<amf_index_t, std::atomic<bool>>& amfs_connected)
{
  return launch_async<amf_connection_removal_routine>(ngap_db.get_ngaps().begin()->second,
                                                      amfs_connected.begin()->second);
}

#endif // SRSRAN_HAS_ENTERPRISE

amf_connection_removal_routine::amf_connection_removal_routine(ngap_interface*    ngap_,
                                                               std::atomic<bool>& amf_connected_) :
  ngap(ngap_), amf_connected(amf_connected_), logger(srslog::fetch_basic_logger("CU-CP"))
{
}

void amf_connection_removal_routine::operator()(coro_context<async_task<void>>& ctx)
{
  CORO_BEGIN(ctx);

  if (ngap == nullptr) {
    logger.error("AMF connection removal routine called with null NGAP interface");
    CORO_EARLY_RETURN();
  }

  // Launch procedure to remove AMF connection.
  CORO_AWAIT(ngap->handle_amf_disconnection_request());

  // Update AMF connection handler state.
  amf_connected = false;

  CORO_RETURN();
}
