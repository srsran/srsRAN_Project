/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "e2sm_kpm_asn1_packer.h"

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
