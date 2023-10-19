/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#pragma once

#include "../ue_context/f1ap_du_ue.h"
#include "srsran/asn1/f1ap/f1ap.h"

namespace srsran {
namespace srs_du {

class f1ap_du_ue_context_modification_procedure
{
public:
  f1ap_du_ue_context_modification_procedure(const asn1::f1ap::ue_context_mod_request_s& msg, f1ap_du_ue& ue_);

  void operator()(coro_context<async_task<void>>& ctx);

private:
  void create_du_request(const asn1::f1ap::ue_context_mod_request_s& msg);
  void send_ue_context_modification_response();
  void send_ue_context_modification_failure();

  f1ap_du_ue&                    ue;
  f1ap_ue_context_update_request du_request;

  f1ap_ue_context_update_response du_response;

  srslog::basic_logger& logger;
};

} // namespace srs_du
} // namespace srsran
