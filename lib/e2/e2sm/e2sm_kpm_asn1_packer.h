/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

  void pack_ran_function_description(asn1::unbounded_octstring<true>& ran_function_description) override;

private:
  std::vector<std::string> supported_metrics = {};
};
} // namespace srsran
