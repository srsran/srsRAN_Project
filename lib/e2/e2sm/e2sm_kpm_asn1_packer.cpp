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

#include "e2sm_kpm_asn1_packer.h"

using namespace asn1::e2ap;
using namespace asn1::e2sm_kpm;
using namespace srsran;

e2_sm_kpm_action_definition_s
e2sm_kpm_asn1_packer::handle_packed_e2sm_kpm_action_definition(const srsran::byte_buffer& action_definition)
{
  e2_sm_kpm_action_definition_s action_def;
  asn1::cbit_ref                bref(action_definition);
  if (action_def.unpack(bref) != asn1::SRSASN_SUCCESS) {
    printf("Failed to unpack E2SM KPM Action Definition\n");
  }
  return action_def;
}

e2_sm_kpm_event_trigger_definition_s
e2sm_kpm_asn1_packer::handle_packed_event_trigger_definition(const srsran::byte_buffer& event_trigger_definition)
{
  e2_sm_kpm_event_trigger_definition_s event_trigger_def;
  asn1::cbit_ref                       bref(event_trigger_definition);
  if (event_trigger_def.unpack(bref) != asn1::SRSASN_SUCCESS) {
    printf("Failed to unpack E2SM KPM Event Trigger Definition\n");
  }
  return event_trigger_def;
}