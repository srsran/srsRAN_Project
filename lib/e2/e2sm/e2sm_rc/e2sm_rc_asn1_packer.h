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

#include "e2sm_rc_provider.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/asn1/asn1_utils.h"
#include "srsran/asn1/e2ap/e2ap.h"
#include "srsran/asn1/e2ap/e2sm_rc.h"
#include "srsran/e2/e2sm/e2sm.h"

namespace srsran {

class e2sm_rc_asn1_packer : public e2sm_handler
{
public:
  static const std::string short_name;
  static const std::string oid;
  static const std::string func_description;
  static const uint32_t    revision;
  e2sm_rc_asn1_packer(e2sm_rc_provider& rc_provider);
  /// Receive populated ASN1 struct that needs to be unpacked and forwarded.
  e2_sm_action_definition_s handle_packed_e2sm_action_definition(const srsran::byte_buffer& action_definition) override;

  e2_sm_event_trigger_definition_s
  handle_packed_event_trigger_definition(const srsran::byte_buffer& event_trigger_definition) override;

  asn1::unbounded_octstring<true> pack_ran_function_description() override;

private:
  void populate_control_ran_function_description(
      e2sm_rc_service_provider                           provider,
      asn1::e2sm_rc::e2_sm_rc_ran_function_definition_s& ran_function_description);
  e2sm_rc_provider& rc_provider;
};
} // namespace srsran
