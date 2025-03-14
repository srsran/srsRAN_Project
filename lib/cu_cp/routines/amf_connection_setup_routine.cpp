/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "amf_connection_setup_routine.h"
#include "srsran/cu_cp/cu_cp_types.h"
#include "srsran/ngap/ngap_setup.h"
#include "srsran/support/async/coroutine.h"

using namespace srsran;
using namespace srs_cu_cp;

#ifndef SRSRAN_HAS_ENTERPRISE

async_task<bool>
srsran::srs_cu_cp::start_amf_connection_setup(ngap_repository&                                    ngap_db,
                                              std::unordered_map<amf_index_t, std::atomic<bool>>& amfs_connected)
{
  amfs_connected.emplace(ngap_db.get_ngaps().begin()->first, false);
  return launch_async<amf_connection_setup_routine>(ngap_db, amfs_connected.begin()->second);
}

#endif // SRSRAN_HAS_ENTERPRISE

amf_connection_setup_routine::amf_connection_setup_routine(ngap_repository&   ngap_db_,
                                                           std::atomic<bool>& amf_connected_) :
  ngap_db(ngap_db_),
  amf_connected(amf_connected_),
  amf_index(ngap_db_.get_ngaps().begin()->first),
  ngap(ngap_db_.get_ngaps().begin()->second),
  logger(srslog::fetch_basic_logger("CU-CP"))
{
}

void amf_connection_setup_routine::operator()(coro_context<async_task<bool>>& ctx)
{
  CORO_BEGIN(ctx);

  if (not ngap->handle_amf_tnl_connection_request()) {
    CORO_EARLY_RETURN(false);
  }

  // Initiate NG Setup.
  CORO_AWAIT_VALUE(result_msg, ngap->handle_ng_setup_request(/*max_setup_retries*/ 1));

  success = std::holds_alternative<ngap_ng_setup_response>(result_msg);

  // Handle result of NG setup.
  handle_connection_setup_result();

  if (success) {
    // Update PLMN lookups in NGAP repository after successful NGSetup.
    ngap_db.update_plmn_lookup(amf_index);

    logger.info("Connected to AMF. Supported PLMNs: {}",
                fmt::format("{}", fmt::join(ngap->get_ngap_context().get_supported_plmns(), " ")));
  } else {
    logger.error("Failed to connect to AMF");
    CORO_EARLY_RETURN(false);
  }

  CORO_RETURN(success);
}

void amf_connection_setup_routine::handle_connection_setup_result()
{
  // Update AMF connection handler state.
  amf_connected = success;
}
