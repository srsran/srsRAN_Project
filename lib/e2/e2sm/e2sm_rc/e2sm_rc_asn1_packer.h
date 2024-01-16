/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/adt/byte_buffer.h"
#include "srsran/asn1/asn1_utils.h"
#include "srsran/asn1/e2ap/e2ap.h"
#include "srsran/asn1/e2ap/e2sm_rc.h"
#include "srsran/e2/e2sm/e2sm.h"
#include <map>

namespace srsran {

class e2sm_rc_asn1_packer : public e2sm_handler
{
public:
  static const std::string short_name;
  static const std::string oid;
  static const std::string func_description;
  static const uint32_t    ran_func_id;
  static const uint32_t    revision;
  e2sm_rc_asn1_packer();
  /// Receive populated ASN1 struct that needs to be unpacked and forwarded.
  e2sm_action_definition   handle_packed_e2sm_action_definition(const srsran::byte_buffer& action_definition) override;
  e2sm_ric_control_request handle_packed_ric_control_request(const asn1::e2ap::ri_cctrl_request_s& req) override;
  e2_ric_control_response  pack_ric_control_response(const e2sm_ric_control_response& e2sm_response) override;

  e2sm_event_trigger_definition
  handle_packed_event_trigger_definition(const srsran::byte_buffer& event_trigger_definition) override;

  asn1::unbounded_octstring<true> pack_ran_function_description() override;

  bool add_e2sm_control_service(e2sm_control_service* control_service);

private:
  std::map<uint32_t, e2sm_control_service*> control_services;
};
} // namespace srsran
