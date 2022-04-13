#include "dl_ssb_pdu.h"
#include "helpers.h"
#include "srsgnb/fapi/validators.h"

using namespace srsgnb;
using namespace fapi;

/// Validates the sfn property of the DL TTI request, as per SCF-222 v4.0 section 3.4.2.
static bool validate_sfn(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 1023;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "sfn", report);
}

/// Validates the slot property of the DL TTI request, as per SCF-222 v4.0 section 3.4.2.
static bool validate_slot(unsigned value, validator_report& report)
{
  static constexpr unsigned MIN_VALUE = 0;
  static constexpr unsigned MAX_VALUE = 159;

  return validate_field(MIN_VALUE, MAX_VALUE, value, "slot", report);
}

error_type<validator_report> srsgnb::fapi::validate_dl_tti_request(const dl_tti_request_message& msg)
{
  validator_report report;
  // Validate SFN and slot.
  bool success = true;
  success &= validate_sfn(msg.sfn, report);
  success &= validate_slot(msg.slot, report);

  // Validate the pdus.
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
  error_type<validator_report> result;
  if (!success) {
    result.set_error(std::move(report));
  }

  return result;
}
