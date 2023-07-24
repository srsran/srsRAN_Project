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

#include "srsran/adt/byte_buffer.h"
#include "srsran/asn1/asn1_utils.h"
#include "srsran/asn1/e2ap/e2ap.h"
#include "srsran/asn1/e2ap/e2sm_kpm.h"
#include "srsran/e2/e2sm/e2sm.h"

namespace srsran {

class e2sm_kpm_asn1_packer : public e2sm_handler
{
public:
  static const std::string short_name;
  static const std::string oid;
  static const std::string func_description;
  static const uint32_t    revision;

  e2sm_kpm_asn1_packer();
  /// Receive populated ASN1 struct that needs to be unpacked and forwarded.
  asn1::e2sm_kpm::e2_sm_kpm_action_definition_s
  handle_packed_e2sm_kpm_action_definition(const srsran::byte_buffer& action_definition) override;

  asn1::e2sm_kpm::e2_sm_kpm_event_trigger_definition_s
  handle_packed_event_trigger_definition(const srsran::byte_buffer& event_trigger_definition) override;

  asn1::unbounded_octstring<true> pack_ran_function_description() override;

private:
  std::vector<std::string> supported_metrics = {};
};
} // namespace srsran
