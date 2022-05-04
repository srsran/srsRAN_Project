#include "srsgnb/fapi/message_validators.h"
#include "dl_ssb_pdu.h"
#include "helpers.h"

using namespace srsgnb;
using namespace fapi;

/// Validates the SFN property of a message.
static bool validate_sfn(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1023;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "sfn", report);
}

/// Validates the slot property of any message.
static bool validate_slot(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 159;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "slot", report);
}

error_type<validator_report> srsgnb::fapi::validate_dl_tti_request(const dl_tti_request_message& msg)
{
  validator_report report(msg.sfn, msg.slot);

  // Validate the SFN and slot.
  bool success = true;
  success &= validate_sfn(msg.sfn, report);
  success &= validate_slot(msg.slot, report);

  // Validate each PDU.
  for (const auto& pdu : msg.pdus) {
    switch (pdu.pdu_type) {
      case dl_pdu_type::SSB:
        success &= validate_dl_ssb_pdu(pdu.ssb_pdu, report);
        break;
      // :TODO: Implement the rest of the PDUs when they validators exist.
      default:
        srsran_assert(0, "Invalid pdu_type");
    }
  }

  // Build the result.
  if (!success) {
    return error_type<validator_report>(std::move(report));
  }

  return {};
}
